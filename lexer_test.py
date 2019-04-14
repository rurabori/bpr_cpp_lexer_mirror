import argparse
import os
import re
import subprocess
import shutil
import difflib
import time
from prettytable import PrettyTable


def find_files(folder, file_filter):
    regex = re.compile(file_filter)
    return [os.path.abspath(basedir+'/'+src) \
                for basedir, _, files in os.walk(folder) \
                    for src in files \
                        if regex.match(src)]

def fake_preprocess(source, *args):
    return os.path.abspath(source)

def real_preprocess(source, destination = '__TEMP_LEXER_PREPROCESSED__.tmp'):
    #... UTF-16, windows, plz stop...
    with    open(source, mode='r') as src, \
            open(destination, mode='w', encoding='ascii') as dest:
        for x in src:
            dest.write(x)
    return os.path.abspath(destination)
    

class lexer:
    def __init__(self, path, identifier = None):
        self.path = path
        self.id = identifier if identifier else self.path.split('/')[-1]

    def lex_file(self, src):
        return [output.strip() \
                    for output in subprocess.check_output(  [self.path, src], \
                                                            stderr=subprocess.STDOUT)
                                                            .decode('ascii', 'ignore')
                                                            .split('\n') \
                        if len(output)]

class lexer_result_accumulator:
    def __init__(self, lexer, preproccess=False):
        self.lexer = lexer
        self.lexed_sources = {}
        self.preprocessor = real_preprocess if preproccess else fake_preprocess

    def lex_file(self, src):
        in_file = self.preprocessor(src)
        start = time.time()          
        try:  
            retval = self.lexer.lex_file(in_file)
        except subprocess.CalledProcessError:
            retval = []
        end = time.time() - start
        self.lexed_sources[src] = end
        return retval

def output_diff(a, b, **kwargs):
    retval = True
    for line in difflib.context_diff(a, b, **kwargs): 
        print(line)
        retval = False

    return retval


class table:
    def __init__(self):
        self.header = []
        self.rows = []

    def __repr__(self):
        table = PrettyTable(self.header)
        for row in self.rows:
            table.add_row(row)
        return str(table)

    def to_csv(self, csv_name):
        with open(csv_name, 'w') as f:
            f.write(','.join(self.header) + '\n')
            for row in self.rows:
                f.write(','.join([str(x) for x in row]) + '\n')

class lexer_tester:
    def __init__(self, base_lexer, other_lexers):
        self.base_lexer = lexer(base_lexer)
        self.other_lexers = [lexer(x) for x in other_lexers]

    def run(self, to_proccess, csv_name):
        base_lexer = lexer_result_accumulator(self.base_lexer)
        other_lexers = [lexer_result_accumulator(x) for x in self.other_lexers]
        
        for src in to_proccess:
            print("Lexing: {}".format(src))
            base = base_lexer.lex_file(src)
            for lexer in other_lexers:
                if not output_diff( base, lexer.lex_file(src),
                                    fromfile=base_lexer.lexer.id, 
                                    tofile=lexer.lexer.id):
                    pass
                    #raise Exception()

        all_lexers = [base_lexer] + other_lexers
        
        results = table()

        results.header = ['source', 'size']+[x.lexer.id for x in all_lexers]

        accumulator = {x.lexer.id : 0 for x in all_lexers}
        for src in to_proccess:
            row = [src , str(os.path.getsize(src))]
            for lexer in all_lexers:
                lex_time = lexer.lexed_sources[src]
                accumulator[lexer.lexer.id] += lex_time
                row.append(lex_time)
            results.rows.append(row)


        results.rows.append(['all', '+++'] + [accumulator[x.lexer.id] for x in all_lexers])

        print(results)

        results.to_csv(csv_name)

def main(args):
    tester = lexer_tester(  os.path.abspath(args.base_lexer), 
                            [os.path.abspath(lexer) for lexer in args.lexers])

    to_process = [os.path.abspath(src) for src in args.files] + \
                 [src for folder in args.folders \
                        for src in find_files(folder, args.folder_filter)]

    tester.run(to_process, args.csv_name)



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='A tool for testing/benchmarking different lexers.')
    parser.add_argument('base_lexer', type=str, help='A path to default lexer, against which the output is matched for correctness.')
    parser.add_argument('--lexers', dest='lexers', type=str, nargs='+', default=[], help='A list of paths to lexers.')
    parser.add_argument('--files', dest='files', type=str, nargs='+', default=[], help='A list of files to lex.')
    parser.add_argument('--folders', dest='folders', type=str, nargs='+', default=[], help='A list of folders to lex.')
    parser.add_argument('--folder_filter', dest='folder_filter', type=str, default='.*', help='A regex to match against filenames.')
    parser.add_argument('--csv_name', dest='csv_name', type=str, default='output.csv', help='A name of csv to output.')

    main(parser.parse_args())