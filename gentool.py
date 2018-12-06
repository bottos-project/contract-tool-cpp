#!/usr/bin/env python

import os, sys, subprocess, time
import copy, shutil
import argparse

# global config:
BIN_PATH = '../bin'
LIB_DIR = '../lib'

struct_array = []
abi_action_array = []
abi_table_container = []

pack_function = list()
unpack_function = list()

def write_each_table(f, head_blank, table_dict, is_last_table):
	inside_blank = '	'

	f.write(head_blank + '{\n')
	f.write(head_blank + inside_blank + '\"table_name\": ' + '\"' + table_dict["table_name"] + '\",\n')
	f.write(head_blank + inside_blank + '\"index_type\": ' + '\"' + table_dict["index_type"] + '\",\n')
	f.write(head_blank + inside_blank + '\"key_names\": ' + ' [\n')
	f.write(head_blank + inside_blank*2 + '\"' + table_dict["key_names"] + '\"\n')
	f.write(head_blank + inside_blank + ' ],\n')
	f.write(head_blank + inside_blank + '\"key_types\": ' + ' [\n')
	f.write(head_blank + inside_blank*2 + '\"' + table_dict["key_types"] + '\"\n')
	f.write(head_blank + inside_blank + ' ],\n')
	f.write(head_blank + inside_blank + '\"type\": ' + '\"' + table_dict["table_type"] + '\"\n')

	if is_last_table:
		f.write(head_blank + '}\n')
	else:
		f.write(head_blank + '},\n')

def write_each_action(f, head_blank, action_dict, is_last_action):
	inside_blank = '	'

	f.write(head_blank + '{\n')
	f.write(head_blank + inside_blank + '\"action_name\": ' + '\"' + action_dict["action_name"] + '\",\n')
	f.write(head_blank + inside_blank + '\"type\": ' + '\"' + action_dict["action_type"] + '\"\n')
	if is_last_action:
		f.write(head_blank + '}\n')
	else:
		f.write(head_blank + '},\n')


def write_each_struct(f, head_blank, struct_info, is_last_struct):

	inside_blank = '	'
	last_info_line_no = len(struct_info)
	curr_line_no = 0

	for each_line in struct_info:
		curr_line_no += 1
		if 'struct ' in each_line and '{' in each_line:
			extra_info = each_line[each_line.find('{'):].strip()
			if len(extra_info) > 1:
				print 'Error! Please check your abi file, struct do not allow any extra info( except blanks ) after \'stuct {\' '
				exit(1)

			struct_name = each_line[each_line.find('struct ') + len('struct ') : each_line.find('{')].strip()
			f.write(head_blank + '{\n')
			f.write(head_blank + inside_blank + '\"name\": ' + '\"' + struct_name + '\",\n')
			f.write(head_blank + inside_blank + '\"base\": \"\",\n')
			f.write(head_blank + inside_blank + '\"fields\": {\n')
			continue

		if '};' in each_line:
			extra_info = each_line[each_line.find('};') + len('};') : ].strip()
			if len(extra_info) > 0:
				print 'Error! Please check your abi file, struct do not allow any extra info( except blanks ) after \'}; in each struct definition\' '
				exit(1)
			f.write(head_blank + '      ' + '}\n')
			if is_last_struct:
				f.write(head_blank + inside_blank + '}\n')
			else:
				f.write(head_blank + inside_blank + '},\n')
			return #struct is end.

		#here are struct body parts=>body inside of '{' and '}'.
		val_list = each_line[ 0 : each_line.find(';')].strip().split()

		valname = ''
		valtype = ''
		for item in val_list:
			if item.strip():
				if not valtype:
					valtype = item.strip()
					if valtype in ('uint8_t', 'int8_t', 'int8', 'uint8', 'char_t', 'char8'):
						valtype = 'uint8'
					elif valtype in ('uint16_t', 'int16_t', 'int16', 'uint16'):
						valtype = 'uint16'
					elif valtype in ('uint32_t', 'int32_t', 'int32', 'uint32', 'int', 'uint'):
						valtype = 'uint32'
					elif valtype in ('uint64_t', 'int64_t', 'int64'):
						valtype = 'uint64'
					elif valtype in ('uint128_t'):
						valtype = 'uint128'
					elif valtype in ('uint256_t'):
						valtype = 'uint256'
				elif not valname:
					valname = item.strip()
					if '[' in valname and ']' in valname:
						if valtype in ('char', 'string'):
							valtype = 'string'
						else:
							valtype = 'bytes'
						valname = valname[0:valname.index('[')]
					break

		inside_blank_for_struct = inside_blank * 2
		if curr_line_no == last_info_line_no - 1:
			f.write(head_blank + inside_blank_for_struct + '\"' + valname + '\": ' + '\"' + valtype + '\"\n')
		else:
			f.write(head_blank + inside_blank_for_struct + '\"' + valname + '\": ' + '\"' + valtype + '\",\n')


def gen_abi_fileEx(abi_path):
	#overwrite mode
	with open(abi_path, 'w') as f:
		f.write('{\n')
		#gen types
		f.write('	' + '\"types\": [],\n')

		#gen abi structs
		f.write('	' + '\"structs\": [\n')
		last_struct_no = len(struct_array)
		curr_struct_no = 0
		for each_struct in struct_array:
			curr_struct_no += 1
			if curr_struct_no == last_struct_no:
				write_each_struct ( f, '              ', each_struct, True )
			else:
				write_each_struct ( f, '              ', each_struct, False )

		f.write('       ' + '],\n')

		#gen abi actions
		f.write('	' + '\"actions\": [\n')
		last_action_no = len(abi_action_array)
		curr_action_no = 0
		for each_action in abi_action_array:
			curr_action_no += 1
			if curr_action_no == last_action_no:
				write_each_action ( f, '              ', each_action, True )
			else:
				write_each_action ( f, '              ', each_action, False )
		f.write('       ' + '],\n')

		#gen abi tables
		f.write('	' + '\"tables\": [\n')
		last_table_no = len(abi_table_container)
		curr_table_no = 0
		for each_table in abi_table_container:
			curr_table_no += 1
			if curr_table_no == last_table_no:
				write_each_table ( f, '              ', each_table, True )
			else:
				write_each_table ( f, '              ', each_table, False )
		f.write('       ' + ']\n')

		f.write('}\n')

def check_virables():
	if len(struct_array) <= 0:
		print 'Error! No struct is defined in your abi file!'
		exit(1)
	if len(abi_action_array) <= 0:
		print 'Error! No abi action is defined in your abi file!'
		exit(1)

def gen_bto_abiEx(hpp_path):
	index_type = ''
        index_type_value = ''

        key_names = ''
        key_names_value = ''

  	key_types = ''
        key_types_value = ''


	struct_lines     = []
	abi_action_lines = []
	abi_table_lines  = []


	abi_table_array  = []

	struct_start     = False
	abi_action_start = False
	abi_table_start  = False

	try:
		with open(hpp_path) as f:
			for line in f.readlines():
				if '@abi table ' in line:
					abi_table_name = line[(line.find('@abi table ') + len('@abi table ')) : line.find(':')].strip()

					if line.find('index_type') < 0 \
					or line.find('key_names') < 0 \
                                        or line.find('key_types') < 0 \
					or line.find('[') < 0 \
					or line.find(']') < 0 :
						print '\nError! \'@abi tale\' section should include [index_type:***, key_names:***, key_types:***].\n'
						exit(1)
					line = line.replace(' ','')

					line_sp = line[line.index('[')+1:line.index(']')].split(',')
					for item in line_sp:
						if 'index_type' in item:
							index_type, index_type_value = item.split(':')
							if len(index_type_value) <= 0:
								print '\nError! \'index_type\' has no value!\n'
						elif 'key_names' in item:
							key_names, key_names_value = item.split(':')
							if len(key_names_value) <= 0:
								print '\nError! \'key_names\' has no value!\n'
						elif 'key_types' in item:
							key_types, key_types_value = item.split(':')
							if len(key_types_value) <= 0:
								print '\nError! \'key_types\' has no value!\n'

					abi_table_start = True
					struct_start = True
					continue
				elif '@abi action ' in line:
					abi_action_name = line[(line.find('@abi action ') + len('@abi action ')) : ].strip()

					if len(abi_action_name) <= 0:
						print '\nError! \'@abi action\' should not be none!'
						exit(1)

					abi_action_start = True
					struct_start = True

					continue
				elif 'struct ' in line and '{' in line:
					if abi_action_start is False and abi_table_start is False:
						struct_start = True
						struct_lines.append(line)
						continue

				if struct_start is True:
					if not line.strip():
						continue
					struct_lines.append(line)

					if abi_action_start is True:
						abi_info = {}
						abi_info["action_name"] = abi_action_name

                                                abi_action_type = line.replace('struct ', '')
						abi_action_type = abi_action_type.replace('{', '').strip()

						abi_info["action_type"] = abi_action_type
						abi_action_array.append(abi_info)

						abi_action_start = False
						abi_action_lines = []

					elif abi_table_start is True:

                                                abi_table_type = line.replace('struct ', '')
						abi_table_type = abi_table_type.replace('{', '').strip()

						abi_table_array_map = dict()
						abi_table_array_map["table_name"] = abi_table_name
						abi_table_array_map["index_type"] = index_type_value
						abi_table_array_map["key_names"]  = key_names_value
						abi_table_array_map["key_types"]  = key_types_value
						abi_table_array_map["table_type"] = abi_table_type

						abi_table_container.append(abi_table_array_map)

						abi_table_lines = []
						index_type_value = ''
						key_names_value = ''
						key_types_value = ''
						abi_table_array = []

						abi_table_start = False
						abi_table_lines = []


					if '};' in line:

						struct_array.append(struct_lines)
						struct_lines = []

						if abi_action_start is True:
							pass
						elif abi_table_start is True:
							pass

						struct_start = False


	except Exception as error:
		s=sys.exc_info()
        	print "Error '%s' happened on line %d" % (s[1],s[2].tb_lineno)
		print 'Error! Process file %s failed: error: %s' %(hpp_path, error)
		return

def help_info():
	print '\n******GENERATE ABI *******\n'
	print 'Parameter: ***.hpp\n'
	print 'Output: ***.abi\n'
	print 'Example: \n\
    cd helloworld\n\
    python ../gentool.py --file helloworld.hpp\n\
    then the [helloworld.abi] will ben generated under the current directory.\n'

	print '\n******GENERATE YOUR COMMON.HPP FILE *******\n'
	print 'Parameter: common ***.hpp\n'
	print 'Output: common.hpp\n'
	print 'Example: \n\
    cd sample_get_set \n\
    python ../gentool.py --type common --file tcase.hpp\n\
    then the [common.hpp] will ben generated under the current directory.\n'
    	print 'Restrictions for generate wasm file\'s cpp \n\n\
    a. Create a new folder to save new contract file.\n\
    b. Your must write \" #include \"common.hpp\" as the last be included hpp header in your **.cpp file\'s include headers.\n\
    b. Be sure to keep "lib" folder, "bin" folder , contract folder and "gentool.py" file under the same path.\n' \
			  'if not,please use --lib and --bin to point the path of "lib" and "bin".'

	print '\n******GENERATE WASM *******\n'
	print 'Parameter: wasm ***.cpp\n'
	print 'Output: ***.wasm\n'
	print 'Example: \n\
    cd helloworld \n\
    python ../gentool.py --type wasm --file helloworld.cpp\n\
    then the [helloworld.wasm] will ben generated under the current directory.\n'
    	print 'Restrictions for generate wasm file\'s cpp \n\n\
    a. Create a new folder to save new contract file.\n\
    b. Your cpp file must include "contractcomm.hpp" at first.\n\
    b. Be sure to keep "lib" folder, "bin" folder , contract folder and "gentool.py" file under the same path.\n ' \
			  'if not,please use --lib and --bin to point the path of "lib" and "bin".'


def excute_cmd(cmd):

        P =subprocess.Popen(cmd, shell=True)
        P.communicate()

def gen_wasm(cpp_filename):
	if not (os.path.exists('%s/clang' %(BIN_PATH)) and os.path.exists('%s/llc' %(BIN_PATH)) and  os.path.exists('%s/llvm-link' %(BIN_PATH)) and os.path.exists('%s/s2wasm' %(BIN_PATH))):
		print 'Error! Please keep these files [clang | llc | llvm-link | s2wasm] under current directory.'
		exit(1)
	if not os.path.exists(cpp_filename):
		print 'Error! File %s does not exist.' %cpp_filename
		exit(1)

	tmpdir = './tmpdir'
	builddir = './tmpdir/build'

	if os.path.isdir(tmpdir):
		shutil.rmtree(tmpdir)

	os.mkdir(tmpdir)
	os.mkdir(builddir)

	if not os.path.isdir(LIB_DIR):
		print 'Error! Could not find [lib] directory in current directory.'
		exit(1)

	gen_build_cmd = '%s/clang -emit-llvm -O3 --std=c++14 --target=wasm32 -ffreestanding -nostdlib -fno-threadsafe-statics -fno-rtti -fno-exceptions -I %s -I . -c %s -o %s/%s' %(BIN_PATH,LIB_DIR, cpp_filename, builddir, cpp_filename)
	print gen_build_cmd
	excute_cmd(gen_build_cmd)

	gen_link_cmd = '%s/llvm-link -o %s/linked.bc %s/*' %(BIN_PATH, tmpdir, builddir)
	print gen_link_cmd
	excute_cmd(gen_link_cmd)

	gen_assembly_cmd = '%s/llc --asm-verbose=false -o %s/assembly.s %s/linked.bc' %(BIN_PATH, tmpdir, tmpdir)
	print gen_assembly_cmd
	excute_cmd(gen_assembly_cmd)

	wast_filename = cpp_filename[:-4] + '.wast'
	gen_wast_cmd = '%s/s2wasm -o %s -s 16384 %s/assembly.s' %(BIN_PATH, wast_filename, tmpdir)
	print gen_wast_cmd
	excute_cmd(gen_wast_cmd)

	wasm_filename = cpp_filename[:-4] + '.wasm'
	gen_wasm_cmd = '%s/wat2wasm %s -o %s' %(BIN_PATH, wast_filename, wasm_filename)
	excute_cmd(gen_wasm_cmd)

	if os.path.isdir(tmpdir):
		shutil.rmtree(tmpdir)

types = ('uint', 'uint8', 'uint16', 'uint32', 'uint64', 'uint128', 'uint256', 'bytes',
	 'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t', 'uint128_t', 'uint256_t',
	 'char', 'string',
	 'bool',
	 'long')

contract_struct_map = dict()
contract_action_map = dict()

contract_structs_name  = dict()
contract_table_map     = dict()
contract_table_struct_map = dict()
special_types_value = dict()

def putdown_pack_code(key, value, pack_function, unpack_function, base):
	if key in ('uint8', 'uint8_t'):
		PACK_VAL = '	PACK_U8'
		UNPACK_VAL = '	UNPACK_U8'
	elif key in ('uint16', 'uint16_t'):
		PACK_VAL = '	PACK_U16'
		UNPACK_VAL = '	UNPACK_U16'
	elif key in ('uint32', 'uint32_t'):
		PACK_VAL = '	PACK_U32'
		UNPACK_VAL = '	UNPACK_U32'
	elif key in ('uint8', 'uint64', 'uint64_t', 'long'):
		PACK_VAL = '	PACK_U64'
		UNPACK_VAL = '	UNPACK_U64'
	elif key in ('uint128', 'uint128_t'):
		PACK_VAL = '	PACK_U128'
		UNPACK_VAL = '	UNPACK_U128'
	elif key in ('uint256', 'uint256_t'):
		PACK_VAL = '	PACK_U256'
		UNPACK_VAL = '	UNPACK_U256'
	elif key in 'bytes':
		PACK_VAL = '	PACK_BIN'
		UNPACK_VAL = '	UNPACK_BIN'
	elif key in 'char':
		PACK_VAL = '	PACK_STR16'
		UNPACK_VAL = '	UNPACK_STR'
		if '[' in value:
			value = value.split('[')[0]
		pack_function.append(PACK_VAL + '(' + base + ', ' + value +')\n')
		unpack_function.append(UNPACK_VAL + '(' + base + ', ' + value +', USER_NAME_LEN + 1)\n')
		return
	elif key in 'string':
		PACK_VAL = '	PACK_STRING'
		UNPACK_VAL = '	UNPACK_STRING'
		if '[' in value:
			value = value.split('[')[0]
		pack_function.append(PACK_VAL + '(' + base + ', ' + value +')\n')
		unpack_function.append(UNPACK_VAL + '(' + base + ', ' + value +',  USER_NAME_LEN + 1)\n')
		return
	else:
		print 'unsupported struct key %s! ' %key
		exit(1)

	pack_function.append(PACK_VAL + '(' + base + ', ' + value +')\n')
	unpack_function.append(UNPACK_VAL + '(' + base + ', ' + value +')\n')

def handle_tuples(tuple_value, pack_function, unpack_function, base='info'):
	is_need_end = True
	for key, value in tuple_value:
		if key in types:
			putdown_pack_code(key, value, pack_function, unpack_function, base)

		elif contract_structs_name.has_key(key):
			is_need_end = False
			tuple_value = contract_structs_name[key]

			if special_types_value.has_key(key):
				special_val_name = special_types_value[key]
			else:
				print 'Error! type [%s] not found!'
				exit(1)
			handle_tuples(tuple_value, pack_function, unpack_function, '(&' + base + '->' + special_val_name + ')')
		else:
			print 'Unsupported struct [%s] in your hpp!' %key
			exit(1)

	if is_need_end == True:
		pack_function.append('	return 1;\n')
		unpack_function.append('	return 1;\n')
		pack_function.append('}\n')
		unpack_function.append('}\n')


def write_common_hpp(action_name, input_hpp, struct_name_map, struct_value_map):

	struct_name = struct_name_map[action_name]
	tuple_value = struct_value_map[action_name]

	pack_function.append('static bool   pack_struct(MsgPackCtx *ctx, '   + struct_name + ' *info) {\n')
	pack_function.append('	uint32_t size = 0;\n')

	pack_function.append('	PACK_ARRAY16(' + str(len(tuple_value)) + ')\n')

	unpack_function.append('static bool unpack_struct(MsgPackCtx *ctx, ' + struct_name + ' *info) {\n')
	unpack_function.append('	uint32_t size = 0;\n')

	unpack_function.append('	UNPACK_ARRAY(' + str(len(tuple_value)) + ')\n')

	handle_tuples(tuple_value, pack_function, unpack_function)

	return copy.copy(pack_function), copy.copy(unpack_function)


def gen_common_hpp(input_hpp, output_hpp):

	with open(input_hpp, 'r') as f:
		fileinfo = f.readlines()

	start  = False
	action = False
	table  = False
	action_name = ''
	struct_name = ''

	for line in fileinfo:
		if '@abi ' and 'action ' in line:
			action_start = True
			action_name = line.split()[2]
			contract_struct_map[action_name] = list()
			continue
		elif '@abi ' and 'table ' in line:
			table_start = True
			table_name = line.split()[2].split(':')[0]
			contract_table_struct_map[table_name] = list()
			continue

		if 'struct ' in	line:
			start = True
			struct_name = line.split()[1]
			contract_structs_name[struct_name] = list()

			continue
		if '{' in line:
			continue
		if '}' in line:
			start = False
			action_start = False
			table_start = False
			continue

		if start is False:
			continue

		code_line = line.split(';')[0].split()
		if len(code_line) != 2:
			print 'error in line [%s] ! Please check your hpp file of %s.' %(line, input_hpp)
			print '\nNote: 1. Do not use macro instead of your variable\'s type definition.'\
			      '	       2. Currently we donnot support: long long | pointer * | reference & in your contract hpp files.\n'
			exit(1)


		if start is True:
			_type, _val = code_line

			if not _type in types:
				special_types_value[_type] = _val

			contract_structs_name[struct_name].append((_type, _val))
			if action_start is True:
				#print 'LINE:', line
				#print 'ACTION NAME: ', action_name, ", APPEND: ", (_type, _val)

				contract_struct_map[action_name].append((_type, _val))
				contract_action_map[action_name] = struct_name

			elif table_start is True:
				contract_table_map[table_name] = struct_name
				contract_table_struct_map[table_name].append((_type, _val))

	for action_key in contract_action_map:
			pack_func, unpack_func = write_common_hpp(action_key, input_hpp, contract_action_map, contract_struct_map)

	if len(pack_function) > 0:
		del pack_function[:]
	if len(unpack_function) > 0:
		del unpack_function[:]

	for table_key in contract_table_map:
			pack_func2, unpack_func2 = write_common_hpp(table_key, input_hpp, contract_table_map, contract_table_struct_map)

	with open(output_hpp, 'w') as f:
		for line in pack_func:
			f.write(line)
		for line in unpack_func:
			f.write(line)
		for line in pack_func2:
			f.write(line)
		for line in unpack_func2:
			f.write(line)

	print '\n===== Put all pack and unpack information into common.hpp done. Please have a check your common.hpp file.\n'


def set_bin_path(full_path,arg_path):
	global BIN_PATH
	if args.bin != '../bin':
		BIN_PATH = arg_path
	else:
		length = len('gentool.py')
		path = full_path[0:len(full_path) - length]
		BIN_PATH = path+'bin'

def set_lib_path(full_path,arg_path):
	global LIB_DIR
	if args.lib != '../lib':
		LIB_DIR = arg_path
	else:
		length = len('gentool.py')
		path = full_path[0:len(full_path)-length]
		LIB_DIR = path+'lib'


if __name__ == '__main__':

	parser = argparse.ArgumentParser(description='manual to this script')
	parser.add_argument('--type', type=str, default=None, help="common or wasm")
	parser.add_argument('--file', type=str, default=None, help="the path of the file which you want to build")
	parser.add_argument('--lib', type=str, default='../lib', help="(optional)the path of lib")
	parser.add_argument('--bin', type=str, default='../bin', help="(optional)the path of bin")

	args = parser.parse_args()

	set_bin_path(sys.argv[0], args.bin)
	set_lib_path(sys.argv[0], args.lib)

	if args.type is not None:
		if args.type == 'wasm':
			if args.file is None:
				help_info()
				exit(1)
			if not os.path.exists(args.file):
				print 'Error! File %s not exist!' %args.file
				exit(1)
			if '.cpp' not in args.file:
				print 'Error! %s is not a cpp file!' %args.file
				exit(1)

			gen_wasm(args.file)
			exit(0)
		if args.type == 'common':
			if args.file is None:
				help_info()
				exit(1)
			if '.hpp' not in args.file:
				print 'Please input your hpp file.'
				exit(1)
			hpp_file = args.file
			gen_common_hpp(hpp_file, 'common.hpp')
			exit(0)

	if args.file is None:
		help_info()
		exit(0)

	if '.hpp' not in args.file:
		help_info()
		exit(1)

	if not os.path.exists(args.file):
		print 'Error! File %s not exist!' % args.file
		exit(1)

	abi_output = args.file[:args.file.find('.hpp')] + '.abi'

	gen_bto_abiEx(args.file)
	check_virables()
	gen_abi_fileEx(abi_output)

	print '\n[ %s ] is generated. Please have a check.\n' % abi_output
	exit(0)

	#print '\n\n====>DONE! struct_array:\n'
	for each_struct in struct_array:
		for each_line in each_struct:
			print each_line

  	#print '\n\n====>DONE! abi_action_array:\n'
	for each_abi_struct in abi_action_array:
		for each_abi_line in each_abi_struct:
			print each_abi_line

  	#print '\n====>DONE! abi_table_container:\n', abi_table_container
	#print 'TRY PRINT:'
	for each_abitable_map in abi_table_container:
		#print '\n|+++++++++++++++++++++++++++++++++|\n'
		for each_item in each_abitable_map.items():
			if 'table_lines' in each_item[0]:
				#print '\n(table_lines: \n'
				for each_table_struct in each_item[1]:
					for each_table_line in each_table_struct:
						print each_table_line,'\n'
				#print ')'
			else:
				print each_item


