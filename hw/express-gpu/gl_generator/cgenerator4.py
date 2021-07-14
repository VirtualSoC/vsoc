#!/bin/python3
# -*- coding: UTF-8 -*-

import sys
from gen_android import *
from gen_qemu import *



# gl_pixel_data_size(width,height,format,type)
filenames = [
    "1-1",
    "1-1-1",  
    "1-1-2", 
    "1-2",  
    "2-1",
    "2-1-1",  
    "2-1-2",  
    "2-2",  
]

# 64-bit target?
target64bit = True

ptrbits = 64 if target64bit else 32
sizeof_dic = {
    'GLboolean': 32,     'GLbyte': 8,           'GLubyte': 8,
    'GLshort': 16,       'GLushort': 16,        'GLint': 32,
    'GLuint': 32,        'GLfixed': 32,         'GLint64': 64,
    'GLuint64': 64,      'GLsizei': 32,         'GLenum': 32,
    'GLintptr': ptrbits, 'GLsizeiptr': ptrbits, 'GLsync': ptrbits,
    'GLeglImageOES': ptrbits,
    'GLbitfield': 32,    'GLhalf': 16,          'GLfloat': 32,
    'GLclampf': 32,      'GLdouble': 64,        'GLclampd': 64,
    'GLchar': 8,         'GLclampx': 32,
    # GLchar is not defined in documents, but is defined as 'char' in header
    # files
}





def sizeof(arg_type):
    return int(sizeof_dic[arg_type] / sizeof_dic['GLbyte'])


class Func:
    def __init__(self, line, file_type):

        self.line = line.strip()

        self.name = ""
        self.ret = ""
        self.args = []
        self.non_ptr_args = []
        self.in_ptr_args = []
        self.out_ptr_args = []  # ptr used as a return value

        self.type = file_type

        self.args_num = 0
        self.send_items_num = 0

        self.PtrOfPtr = False

        self.read_def()

    def read_def(self):

        # print(self.line)
        temp=self.line.split("@")
        if len(temp)>1:
            self.check_error=temp[1]
        else:
            self.check_error=""


        temp = temp[0].split()

        # Check if there is a return value
        offset = 0
        if temp[0][:2] != 'gl':
            if temp[0] == 'const':
                self.ret = ' '.join(temp[:2])
                offset = 2
            else:
                self.ret = temp[0]
                offset = 1

        # Get function name
        self.name = temp[offset]
        offset += 1

        # Read arguments
        if temp[-1] != 'void':
            flag_out = 0
            argu_cnt = 0
            while offset < len(temp):
                ptr = 'NA'
                ptr_len = 'NA'

                # Const arguments are inputs
                if temp[offset] == 'const':
                    if flag_out == 1:
                        print("error!!!! out before in!!!")
                    typ = ' '.join(temp[offset:offset+2]) + "*"
                    name = temp[offset+2][1:]
                    if name[-1] == ',':
                        name = name[:-1]
                    ptr = 'in'
                    offset += 3
                    if name.find("#") != -1:
                        name_len = name.split("#")
                        name = name_len[0]
                        ptr_len = name_len[1]
                    # if name[:6] == 'const*':
                    #     self.errorParsing.StarConstStar = True

                else:
                    typ = temp[offset]
                    # print(temp)
                    name = temp[offset+1]
                    if name[-1] == ',':
                        name = name[:-1]
                    offset += 2
                    if name[0] == '*':
                        flag_out = 1
                        ptr = 'out'
                        typ += "*"
                        name = name[1:]
                        if name.find("#") != -1:
                            name_len = name.split("#")
                            name = name_len[0]
                            ptr_len = name_len[1]

                # ptr of ptr

                PtrOfPtr = False
                if name[:6] == 'const*':
                    self.PtrOfPtr = True
                    PtrOfPtr = True
                    name = name[6:]

                # while name[0] == '*':
                #     self.errorParsing.PtrOfPtr = True
                #     typ += "*"
                #     name = name[1:]

                if name[-1] == ',':
                    name = name[:-1]

                self.args += [{'type': typ, 'name': name, 'ptr': ptr,
                               'ptr_len': ptr_len, "loc": argu_cnt, 'ptr_ptr': PtrOfPtr}]
                argu_cnt += 1

            self.non_ptr_args = [
                arg for arg in self.args if arg['ptr'] == 'NA']
            self.in_ptr_args = [arg for arg in self.args if arg['ptr'] == 'in']
            self.out_ptr_args = [
                arg for arg in self.args if arg['ptr'] == 'out']
            self.args_num = len(self.args)
            # self.send_items_num = len(self.in_ptr_args)
            # self.send_items_num += len(self.out_ptr_args)
            # if len(self.non_ptr_args) != 0:
            #     self.send_items_num += 1

            # if self.ret != "":
            #     self.send_items_num += 1

    def get_non_ptr_arg_length(self):
        # Get size of non ptr arguments
        non_ptr_arg_length = 0
        for arg in self.non_ptr_args:
            non_ptr_arg_length += sizeof(arg['type'])
        return non_ptr_arg_length



    def generate_android_header_local(self,head_file,local_file,id_define):

        # Write definition string
        def_str1 = ""
        def_str2= ""
        if self.ret != '':
            def_str1 += self.ret + ' '
        else:
            def_str1 += 'void '
        def_str2=def_str1

        def_str1 += f"r_{self.name}(void *context, "
        def_str2 += f"d_{self.name}(void *context, "

        for arg in self.args:
            if arg['ptr_ptr']:
                def_str1 += f"{arg['type']}const* {arg['name']}, "
                def_str2 += f"{arg['type']}const* {arg['name']}, "

            else:
                def_str1 += f"{arg['type']} {arg['name']}, "
                def_str2 += f"{arg['type']}const* {arg['name']}, "

        # if len(self.args) > 0:
        def_str1 = def_str1[:-2]  # Remove last ', '
        def_str2 = def_str2[:-2]  # Remove last ', '

        head_file.write(id_define)

        head_file.write(def_str1+');\n')

        head_file.write(def_str2+');\n\n')

        local_fun_str=""
        local_fun_str+=def_str2
        local_fun_str+=")\n{\n"
        local_fun_str+=self.check_error+"\n"

        if self.ret != '':
            local_fun_str += 'return '

        local_fun_str+= f"r_{self.name}(context, "

        for arg in self.args:
            local_fun_str += f"{arg['name']}, "
        # if len(self.args) > 0:
        local_fun_str = local_fun_str[:-2]  # Remove last ', '
        local_fun_str +=');\n'

        local_fun_str +=  '\n}\n\n\n'
        local_file.write(local_fun_str)


        return def_str1+"){\n"


    def generate_android_func(self,header_file,local_file,out_file,id_define):
        # Debug information

        ret_str=self.generate_android_header_local(header_file,local_file,id_define)

        out_file.write("\n")
        out_file.write(f"/* readline: \"{self.line}\" */\n")
        out_file.write(f"/* func name: \"{self.name}\" */\n")
        out_file.write(f"/* args: {self.args} */\n")
        out_file.write(f"/* ret: \"{self.ret}\" */\n")
        out_file.write(f"/* type: \"{self.type}\" */\n")
        out_file.write(ret_str)
        out_file.write("\n")

        if self.type//100==1:
            if self.type%100==10:
                gen_android_sync(self,out_file,True)
            elif self.type%100==20:
                gen_android_sync(self,out_file,False)
            elif self.type%100==12:
                gen_android_sync(self,out_file,True)
            elif self.type%100==11:
                pass
        elif self.type//100==2:
            if self.type//10==21:
                gen_android_asyn_copy(self,out_file)
            elif self.type//10==22:
                gen_android_asyn_no_copy(self,out_file)
                
        # End of function
        out_file.write("}\n\n")




    def generate_qemu_case(self, out_file, define_file,define_id):


        define_file.write(define_id+"\n")
        define_file.write(f"#define PARA_NUM_MIN_{self.name} ({self.send_items_num})\n\n")

        # Case
        out_file.write(f"case FUNID_{self.name}:\n\n{{\n")
        # Debug information
        out_file.write("\n")
        out_file.write(f"/* readline: \"{self.line}\" */\n")
        out_file.write(f"/* func name: \"{self.name}\" */\n")
        out_file.write(f"/* args: {self.args} */\n")
        out_file.write(f"/* ret: \"{self.ret}\" */\n")
        out_file.write(f"/* type: \"{self.type}\" */\n")
        out_file.write("\n")

        # If there are more than one ptr, require checking
        if len(self.in_ptr_args) + len(self.out_ptr_args) > 1:
            out_file.write(
                f"/* TODO: More than one ptr, should check mannually */\n")

        # Define non-pointer variables
        out_file.write("/* Define variables */\n")
        for arg in self.non_ptr_args:
            out_file.write(f"{arg['type']} {arg['name']};\n")
        header_str = \
            f"""
int para_num=get_para_from_call(call,all_para,MAX_PARA_NUM);
if(para_num<PARA_NUM_MIN_{self.name}){{
    break;
}}


"""
        out_file.write(header_str)

        if self.type//100==1:
            if self.type%100==10 or self.type%100==12:
                gen_qemu_sync(self,out_file,True)
            elif self.type%100==20:
                gen_qemu_sync(self,out_file,False)
            elif self.type%100==11:
                pass
        elif self.type//100==2:
            if self.type%100==10 or self.type%100==12:
                gen_qemu_asyn_copy(self,out_file,False)
            elif self.type%100==11:
                gen_qemu_asyn_copy(self,out_file,True)
            elif self.type%100==20:
                gen_qemu_asyn_no_copy(self,out_file)

        
        out_file.write("}\nbreak;\n\n\n")



        




define_id_async = 1
define_id_sync = 0

all_func_count = 0


qemu_out = open(f"qemu/all_out.c", "w+")
android_out = open(f"android/all_out.cpp", "w+")

android_local_out = open("android/guest_handle.cpp",'w+')

qemu_define = open(f"qemu/define.h", "w+")
android_define=open(f"android/define.h","w+")

all_include = open(f"all_gl.h", "w+")

all_include_dict={}

android_out.write("""
#include "define_gl.h"


#include "glv3_mem.h"
#include "glv3_status.h"
#include "glv3_texture.h"
#include "glv3_utils.h"
#include "glv3_vertex.h"

#include "../context_common.h"
\n""")


android_local_out.write("""
#include "define_gl.h"
#include "utils_gl.h"


//#include "all_gl.h"

#include <cstring>

\n""")



qemu_out.write(
    """

#include "glad.h"
#include "glext.h"
#include "gl3.h"
#include "gl.h"

#include "define.h"


void gl3_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call)
{
    Render_Thread_Context *render_context=(Render_Thread_Context *)context;
    Opengl_Context *opengl_context=&(render_context->opengl_context);
    //uint64_t fun_id=GET_FUN_ID(call->id);
    //uint64_t is_async=FUN_IS_ASYNC(call->id);
    //uint64_t need_speed=FUN_NEED_SPEED(call->id);
    Call_Para all_para[MAX_PARA_NUM];

    unsigned char ret_local_buf[1024*4];

    unsigned char no_ptr_buf[512];
    
    switch (call->id)
{\n""")

qemu_define.write("""


#ifndef GLV3_TRANS_H
#define GLV3_TRANS_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/offscreen_render_thread.h"


#define MAX_OUT_BUF_LEN 4096
""")



android_define.write("""

#ifndef DEFINE_GL_H
#define DEFINE_GL_H

//android
#include <GLES/glplatform.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2platform.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>


# define EXPRESS_GPU_FUN_ID ((unsigned long long)1)


#define MAX_OUT_BUF_LEN 4096\n
""")

define_id = 1
for filename in filenames:
    file_type = 0
    if filename == "1-1":
        file_type = 110
    elif filename == "1-1-1":
        file_type = 111
    elif filename == "1-1-2":
        file_type = 112
    elif filename == "1-2":
        file_type = 120
    elif filename == "2-1":
        file_type = 210
    elif filename == "2-1-1":
        file_type = 211
    elif filename == "2-1-2":
        file_type = 212
    elif filename == "2-2":
        file_type = 220





    input_file = open(f"in4/{filename}.txt", "r", encoding="utf-8")

    # qemu_out = open(f"qemu/{filename}.h", "w+")

    qemu_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    android_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    android_local_out.write(f"\n\n/******* file '{filename}' *******/\n\n\n")
    

    func_count = 1
    for line in input_file:



        # Empty line means end of file
        if line.strip() == '' or line[0:2] == "//":
            continue
        # print(line)
        # Get func defination
        func = Func(line, file_type)
        func_count += 1
        all_func_count += 1

        flag_str=""
        if func.type//100==1:
            flag_str+="+(((unsigned long long)0x1)<<24u)"
        
        define_id_str = f"#define FUNID_{func.name} ((EXPRESS_GPU_FUN_ID<<32u){flag_str}+{define_id})\n"

        real_name=func.name
        if func.name.find("_")!=-1 and func.name.find("_v")==-1:
            loc=func.name.find("_")
            real_name=func.name[:loc]
        if real_name not in all_include_dict:
            all_include_dict[real_name]=1
            all_include.write(real_name+"\n")

        # Work
        func.generate_qemu_case(qemu_out,qemu_define, define_id_str)
        func.generate_android_func(android_define,android_local_out,android_out,define_id_str)
                    

        define_id += 1

    # End for
    qemu_out.write(f"\n\n/******* end of file '{filename}', "
                   f"{func_count}/{all_func_count} functions*******/\n\n\n")
    android_out.write(f"\n\n/******* end of file '{filename}', "
                      f"{func_count}/{all_func_count} functions *******/\n\n\n")
    input_file.close()
    # qemu_out.close()

qemu_out.write(
    """    default:
                break;
    }

    //if(need_speed){
    call->callback(call, 1);
    //}else{
    //    call->callback(call, 0);
    //}
    return;
}\n""")





qemu_define.write("""
void gl3_decode_invoke(Render_Thread_Context *context,Direct_Express_Call *call);

#endif
""")




android_define.write("\n\n\n\n\n\n\n\n")
android_define.write("""
#endif
""")



all_include.close()

qemu_out.close()
qemu_define.close()
# qemu_para_num.close()
