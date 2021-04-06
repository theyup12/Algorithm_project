
# Needs Python >= 3.7

import hashlib, json, pathlib, subprocess, xml.etree.ElementTree

REJECTED = False

REJECTION_MESSAGE = ''

CONTRIBUTORS_PATH = 'README.md'

PLACEHOLDER_CONTRIBUTORS = ['Ada Lovelace adalovelace@csu.fullerton.edu',
                            'Charles Babbage charlesbab@csu.fullerton.edu']

REJECTED_TEST_MESSAGE = 'submission was rejected, so test was skipped'

DEFAULT_XML_FILENAME = 'gtest.xml'

AUTOGRADE_JSON_FILENAME = 'results.json'

DEFAULT_TIMEOUT = 60

REJECTED_SCORE_FRACTION = .5

INDENT = ' ' * 4

def reject(message):
    global REJECTED
    global REJECTION_MESSAGE
    print('REJECTED: ' + message)
    REJECTED = True
    REJECTION_MESSAGE = message

def contributors():
    lines = []
    with open(CONTRIBUTORS_PATH, 'r') as f:
        for line in f.readlines():
            i = line.find('@')
            if i not in [-1, 0, len(line)-1]:
                lines.append(line.strip())
    return lines
        
def sha256(path):
    hash = hashlib.sha256()
    with open(path, 'rb') as f:
        hash.update(f.read())
    return hash.hexdigest()

def reject_if_file_unchanged(filename, original_sha256_hex):
    if not (isinstance(filename, str) and
            isinstance(original_sha256_hex, str)):
        raise TypeError
    if sha256(filename) == original_sha256_hex:
        reject('file "' + filename + '" is unchanged')

def reject_unless_file_exists(filename):
    if not isinstance(filename, str):
        raise TypeError
    if not pathlib.Path(filename).exists():
        reject('required file "' + filename + '" does not exist')

def reject_unless_files_exist(filename_list):
    if not (isinstance(filename_list, list) and
            all([isinstance(x, str) for x in filename_list])):
        raise TypeError
    for filename in filename_list:
        reject_unless_file_exists(filename)
        
def reject_if_no_contributors():
    if len(contributors()) == 0:
        reject('no names and emails in ' + CONTRIBUTORS_PATH)

def reject_if_placeholder_contributors():
    if set(contributors()) == set(PLACEHOLDER_CONTRIBUTORS):
        reject('names and emails in ' + CONTRIBUTORS_PATH + ' are still the placeholders ' + ' '.join(PLACEHOLDER_CONTRIBUTORS))

def is_args(obj):
    return (isinstance(obj, list) and
            len(obj) > 0 and
            all([isinstance(x, str) and len(x) > 0 for x in obj]))

class Execution:

    RESULTS = SUCCESS, FAILURE, TIMEOUT = range(3)
    
    def __init__(self, result, exit_code, output):
        if not ((result in Execution.RESULTS) and
                isinstance(exit_code, int) and
                isinstance(output, str)):
            raise TypeError
        self.result = result
        self.exit_code = exit_code
        self.output = output

def execute(args, timeout=DEFAULT_TIMEOUT, stdin_string=None):
    if not (is_args(args) and
            isinstance(timeout, int) and
            stdin_string is None or isinstance(stdin_string, str)):
        raise TypeError
    if not (timeout > 0):
        raise ValueError

    print(' '.join(args))    
    try:
        proc = subprocess.run(args,
                              stdout=subprocess.PIPE,   # capture stdout
                              stderr=subprocess.STDOUT, # merge stderr into stdout
                              text=True,                # stdout is text not binary
                              input=stdin_string,
                              timeout=timeout)
        # if we got here, there was no timeout
        exit_code = proc.returncode
        return Execution(Execution.SUCCESS if exit_code == 0 else Execution.FAILURE,
                         exit_code,
                         proc.stdout)
    except subprocess.TimeoutExpired:
        return Execution(Execution.TIMEOUT, 128, '')
    
def reject_unless_command_succeeds(args, timeout=DEFAULT_TIMEOUT, stdin_string=''):
    ex = execute(args, timeout, stdin_string)
    if ex.result == Execution.FAILURE:
        reject('command "' + ' '.join(args) + '" failed with exit code ' + str(ex.exit_code))
    elif ex.result == Execution.TIMEOUT:
        reject('command "' + ' '.join(args) + '" timed out after ' + str(command.timeout) + ' seconds')

class Test:
    def __init__(self, name, max_score, score, message):
        self.name = name
        self.max_score = max_score
        self.score = score
        self.message = message

TESTS = []
        
def test(display_name, max_score, score, message):
    if not (isinstance(display_name, str) and
            isinstance(max_score, int) and
            (isinstance(score, int) or isinstance(score, float)) and
            isinstance(message, str)):
        raise TypeError
    TESTS.append(Test(display_name, max_score, score, message))

def total_max_score():
    return sum([test.max_score for test in TESTS])

def total_score():
    if REJECTED:
        return total_max_score() * REJECTED_SCORE_FRACTION
    else:
        return sum([test.score for test in TESTS])
    
def clang_tidy_test(display_name, max_score, filenames,
                    args=['-checks=*', '--format-style=google']):

    if not (isinstance(display_name, str) and
            isinstance(max_score, int) and
            isinstance(filenames, list) and
            all([isinstance(x, str) for x in filenames])):
        raise TypeError

    if not (all([pathlib.Path(x).exists() for x in filenames])):
        raise ValueError
    
    if REJECTED:
        test(display_name, max_score, 0, REJECTED_TEST_MESSAGE)
        return

    ex = execute(['clang-tidy'] + args + filenames)
    score = max_score if ex.result == Execution.SUCCESS else 0
    test(display_name, max_score, score, ex.output)

def string_removed_test(name, max_score, taboo_string, filenames):
    if not (isinstance(max_score, int) and
            isinstance(taboo_string, str) and
            isinstance(filenames, list) and
            all([isinstance(x, str) for x in filenames])):
        raise TypeError

    passed = True
    message = ''
    for filename in filenames:
        line_numbers = []
        with open(filename, 'rt') as f:
            lines = f.readlines()
            for i in range(len(lines)):
                if lines[i].find(taboo_string) != -1:
                    line_numbers.append(i+1)
                    
        if len(line_numbers) > 0:
            passed = False
            message += (filename +
                        ' still contains "' + taboo_string + '"' +
                        ' on lines ' +
                        ', '.join([str(n) for n in line_numbers]))
    test(name,
         max_score,
         max_score if passed else 0,
         message)
    
def gtest_run(executable_filename,
              timeout=DEFAULT_TIMEOUT,
              xml_filename=DEFAULT_XML_FILENAME):
    if not (isinstance(executable_filename, str) and
            isinstance(timeout, int) and
            isinstance(xml_filename, str)):
        raise TypeError
    
    ex = execute(['./' + executable_filename,
                   '--gtest_output=xml:' + xml_filename],
                 timeout=timeout)

    if ex.result == Execution.TIMEOUT:
        reject('unittest program "' + executable_filename + '" ' +
               'timed out after ' + str(timeout) + ' seconds')

    # bitmask 128 indicates abnormal termination (segfault)
    elif (ex.result == Execution.FAILURE) and ((ex.exit_code & 128) != 0):
        reject('unittest program "' + executable_filename + '" ' +
               'terminated abnormally (crashed):\n\n' + ex.output)

class GTestSuiteResult:
    def __init__(self, name, passed, message):
        if not (isinstance(name, str) and
                isinstance(passed, bool) and
                isinstance(message, str)):
            raise TypeError
        self.name = name
        self.passed = passed
        self.message = message

# Returns a dict d such that d[name] = GTestSuiteResult
def parse_gtest_suite_results(filename):
    if not isinstance(filename, str):
        raise TypeError
    
    result = {}
    
    tree = xml.etree.ElementTree.parse(filename)
    root = tree.getroot()
    for testsuite in root:

        name = testsuite.get('name')
        failures = int(testsuite.get('failures'))

        message = ''
        for testcase in testsuite:
            for failure in testcase:
                message += failure.text
        
        passed = (failures == 0)
        
        result[name] = GTestSuiteResult(name, passed, message)

    return result
        
def gtest_suite(name, max_score, xml_filename=DEFAULT_XML_FILENAME):

    if not (isinstance(name, str) and
            isinstance(max_score, int) and
            isinstance(xml_filename, str)):
        raise ValueError
    
    if REJECTED:
        test(name, max_score, 0, REJECTED_TEST_MESSAGE)
        return

    suites = parse_gtest_suite_results(xml_filename)
    if name not in suites:
        raise ValueError('no such suite name "' + suite_name + '" in "' + xml_filename + '"')

    suite = suites[name]

    if suite.passed:
        test(name, max_score, max_score, '')
    else:
        test(name, max_score, 0, suite.message)

def banner():
    print('-' * 79)

def indent(s):
    if not isinstance(s, str):
        raise TypeError
    result = ''
    for line in s.splitlines():
        result += INDENT + line + '\n'
    return result
    
def print_summary():
    banner()
    print('CONTRIBUTORS:')
    for c in contributors():
        print(INDENT + c)
    print('')

    banner()
    print('SCORE:\n')
    
    if REJECTED:
        print('TOTAL SCORE = ' + str(total_score()) +
              ' / ' + str(total_max_score()) +
              ' (SUBMISSION REJECTED)')
    else:
        name_width = max([len(test.name) for test in TESTS])
        point_width = max([len(str(test.max_score)) for test in TESTS])
        
        for test in TESTS:
            print(test.name.ljust(name_width) + INDENT + ': ' +
                  str(test.score).rjust(point_width) + ' / ' +
                  str(test.max_score).rjust(point_width))
            if len(test.message) > 0:
                print(indent(test.message))

        print('\nTOTAL SCORE = ' + str(total_score()) +
              ' / ' + str(total_max_score()))

    banner()

def autograde_summary():
    d = {'visibility': 'visible',
         'stdout_visibility': 'visible'}
    
    if REJECTED:
        d['score'] = total_score()
        d['output'] = REJECTION_MESSAGE
    else:
        d['tests'] = [ { 'score': test.score,
                         'max_score': test.max_score,
                         'name': test.name,
                         'output': test.message,
                         'visibility': 'visible'}
                       for test in TESTS]

    with open(AUTOGRADE_JSON_FILENAME, 'wt') as f:
        json.dump(d, f)    

banner()

reject_if_no_contributors()
reject_if_placeholder_contributors()

reject_unless_files_exist(['algorithms.hpp'])

reject_if_file_unchanged('algorithms.hpp',
                         '1ce0584ba2296dc9912acce3b852e70c82389abc6e087e15124b5554c901f5b0')

reject_unless_command_succeeds(['make', 'clean', 'algorithms_test'])

string_removed_test('TODO comments removed', 3, 'TODO', ['algorithms.hpp'])

gtest_run('algorithms_test')
gtest_suite('find_dip_trivial_cases', 3)
gtest_suite('find_dip_nontrivial_cases', 3)
gtest_suite('longest_balanced_span_trivial_cases', 3)
gtest_suite('longest_balanced_span_nontrivial_cases', 3)
gtest_suite('telegraph_style_trivial_cases', 3)
gtest_suite('telegraph_style_nontrivial_cases', 3)
        
print_summary()
autograde_summary()
