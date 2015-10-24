#include <stdio.h>
#include <getopt.h>
#include <string>
#include <vector>
#include <algorithm>
#include "ezxml.h"
#include "rpc_log.h"

#define T "    "
#define TT "        "
#define TTT "            "
#define TTTT "                "

/**
 * $id$
 * $name$
 * $version$
 * $dispatch$
 * $stub$
 */

/* param_t */
struct param_t {
    int index;
    std::string name;
    std::string type;
};

/* procedure_t */
struct procedure_t {
    int id;
    std::string name;
    std::string rettype;
    std::vector<param_t> params;
};

/* program_t */
struct program_t {
    int id;
    std::string name;
    std::string version;
    std::string ds_ip;
    unsigned short ds_port;
    std::vector<procedure_t> procedures;
};

static std::string num_to_str(int num) {
    char buf[1024] = { 0 };
    sprintf(buf, "%d", num);
    return buf;
}

static void usage(int argc, char *argv[]) {
    printf("Usage: %s [options]\n", argv[0]);
    printf("-h/--help:      show this help\n");
    printf("-x/--xml:       specify the idl xml file\n");
    printf("-t/--target:    specify the target, can be client_stub|server_stub\n");
    printf("-p/--path:      path for saving files\n");
}

#define FILE_WRITELN(fp, format, args...) \
    fprintf(fp, format"\n", ##args)
    //fprintf(stdout, format"\n", ##args)

#define FILE_WRITE(fp, format, args...) \
    fprintf(fp, format, ##args)
    //fprintf(stdout, format, ##args)

static void file_writeln(FILE *fp, const std::string &line) {
    //puts((line).c_str());
    fputs((line + "\n").c_str(), fp);
}

static void file_write(FILE *fp, const std::string &line) {
    fprintf(fp, "%s", (line).c_str());
    //fprintf(stdout, "%s", (line).c_str());
}

static std::string generate_param(const param_t &param) {
    char buf[1024] = { 0 };
    if (param.type == "Matrix") {
        sprintf(buf, "int *%s, int %s_row, int %s_col", param.name.c_str(), param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "Array" ) {
        sprintf(buf, "int %s_len, int *%s", param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "String") {
        sprintf(buf, "char *%s, int %s_len", param.name.c_str(), param.name.c_str());
    } 
    else {
        sprintf(buf, "%s %s", param.type.c_str(), param.name.c_str());
    }
    return buf;
}

static std::string gen_svr_param_decl(const param_t &param) {
    char buf[1024] = { 0 };
    if (param.type == "Matrix") {
        sprintf(buf, "int *%s; int %s_row; int %s_col;", param.name.c_str(), param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "Array" ) {
        sprintf(buf, "int %s_len; int *%s;", param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "String") {
        sprintf(buf, "char *%s; int %s_len;", param.name.c_str(), param.name.c_str());
    } 
    else {
        sprintf(buf, "%s %s;", param.type.c_str(), param.name.c_str());
    }
    return buf;
}

static std::string gen_svr_param(const param_t &param) {
    char buf[1024] = { 0 };
    if (param.type == "Matrix") {
        sprintf(buf, "%s, %s_row, %s_col", param.name.c_str(), param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "Array" ) {
        sprintf(buf, "%s_len, %s", param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "String") {
        sprintf(buf, "%s, %s_len", param.name.c_str(), param.name.c_str());
    } 
    else {
        sprintf(buf, "%s", param.name.c_str());
    }
    return buf;
}

static std::string gen_svr_param_unmarshalling(const param_t &param) {
    char buf[1024] = { 0 };
    if (param.type == "Matrix") {
        sprintf(buf, "proto_in.read_matrix(%s, %s_row, %s_col);", param.name.c_str(), param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "Array" ) {
        sprintf(buf, "proto_in.read_array(%s, %s_len);", param.name.c_str(), param.name.c_str());
    }
    else if (param.type == "String") {
        sprintf(buf, "proto_in.read_string(%s_len, %s);", param.name.c_str(), param.name.c_str());
    }
    else {
        sprintf(buf, "proto_in.read_%s(%s);", param.type.c_str(), param.name.c_str());
    }
    return buf;
}

static std::string gen_svr_param_marshalling(const param_t &param) {
    char buf[1024] = { 0 };
    if (param.type == "Matrix") {
        sprintf(buf, "proto_out.add_matrix(%s, %s_row, %s_col);", param.name.c_str(), param.name.c_str(), param.name.c_str());
    } 
    else if (param.type == "Array" ){
        sprintf(buf, "proto_out.add_array(%s, %s_len);", param.name.c_str(), param.name.c_str());
    }
    else if (param.type == "String") {
        sprintf(buf, "proto_out.add_string(%s_len, %s);", param.name.c_str(), param.name.c_str());
    }
    else {
        sprintf(buf, "proto_out.add_%s(%s);", param.type.c_str(), param.name.c_str());
    }
    return buf;
}

static bool comp_index(const param_t &a, const param_t &b) {
    return a.index < b.index;
}

/**
 * @brief generate common head file
 *
 * @param xml_filename
 * @param filename
 */

static void gen_req_rsp(FILE *fp, std::string proc_name){
    file_writeln(fp, std::string("\tstd::string rsp_head, rsp_body;"));
    file_writeln(fp, std::string("\trpc_call_by_id(") + "id_" + proc_name + ", " + "svr_inst.ip" + ", " + "svr_inst.port, " + "inpro, " + "rsp_head, " + "rsp_body);");
    file_writeln(fp, std::string("\tbasic_proto outpro(rsp_body.data(), rsp_body.size());"));
    //std::string nameStr = name;
    //file_writeln(fp, std::string("std::string req_head = gen_http_head(") + "\"" + "/" + nameStr + "\"" + ", svr_inst.ip, inpro.get_buf_len());");
    //file_writeln(fp, std::string("std::string req_body(inpro.get_buf(), inpro.get_buf_len());"));
    //file_writeln(fp, std::string("std::string rsp_head, rsp_body;"));
    //file_writeln(fp, std::string("http_talk(svr_inst.ip, svr_inst.port, req_head, req_body, rsp_head, rsp_body);"));
    //file_writeln(fp, std::string("basic_proto outpro(rsp_body.data(), rsp_body.size());"));
}

static void generate_client_content_stub(FILE *fp, std::string proc_name, const char *name, const char *ret_type, std::vector<param_t> params){
    std::string return_type = ret_type;
    file_writeln(fp, " { \n");
    /* request server */
    file_writeln(fp, std::string("\tsvr_inst_t svr_inst;"));
    std::string ip_str;
    std::string ss = std::string("\tint ret_val = get_and_verify_svr(DS_IP, DS_PORT, RPC_ID, RPC_VERSION, svr_inst);");
    file_writeln(fp, ss);
    std::string error = std::string("\tif(ret_val == -1){");
    file_writeln(fp, error);
    std::string error_content = std::string("\t\tRPC_WARNING(\"VERSION DISMATCH.\");");
    file_writeln(fp, error_content);
    if(return_type == "int"){
        file_writeln(fp, std::string("\t\treturn 0;"));
    }
    else if(return_type == "double"){
        file_writeln(fp, std::string("\t\treturn 0.0;"));
    }
    else if(return_type == "float"){
        file_writeln(fp, std::string("\t\treturn 0.0;"));
    }
    else{
        file_writeln(fp, std::string("\t\treturn;"));
    }

    file_writeln(fp, std::string("\t}"));
    file_writeln(fp, std::string("\tRPC_INFO(")+ "\"" + "server verified, id=%d, version=%s, ip=%s, port=%u\"" + ", " + "svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst.port);");
    file_writeln(fp, "\tbasic_proto inpro;");
    std::string nameStr = name;
    for (int i = 0; i < params.size(); ++i){
        param_t &param = params[i];
        if (param.type == "Matrix"){
            file_write(fp, std::string("\tinpro.add_matrix("));
            file_write(fp, param.name + ", " + param.name + "_row," + param.name + "_col");
            file_writeln(fp,std::string(");"));
        }
        else if (param.type == "Array"){
            file_write(fp, std::string("\tinpro.add_array("));
            file_writeln(fp, param.name + ", " + param.name + "_len);");
        }
        else if (param.type == "int"){
            file_write(fp, std::string("\tinpro.add_int("));
            file_writeln(fp, param.name + ");");
        }
        else if (param.type == "double"){
            file_write(fp, std::string("\tinpro.add_double("));
            file_writeln(fp, param.name + ");");
        }
        else if (param.type == "float"){
            file_write(fp, std::string("\tinpro.add_float("));
            file_writeln(fp, param.name + ");");
        }
        else if (param.type == "String"){
            file_writeln(fp, std::string("\tstd::string ") + param.name +"_str = " + param.name + ";");
            file_writeln(fp, std::string("\tinpro.add_string(" + param.name +"_str" + ".size(), " + param.name +"_str" + ".data());"));
        }
    }
    gen_req_rsp(fp, proc_name);
    for (int i = 0; i < params.size(); ++i){
        param_t &param = params[i];
        if (param.type == "Matrix"){
            file_writeln(fp, std::string("\tint ") + "*" + param.name + "_bak;");
            file_writeln(fp, std::string("\toutpro.read_matrix(") + param.name + "_bak, " + param.name + "_row, " + param.name + "_col);");
            file_writeln(fp, std::string("\tmemcpy(") + param.name + ", " + param.name + "_bak, " + "sizeof(int) * " + param.name + "_row" + " * " + param.name + "_col);");
        }
        else if (param.type == "Array"){
            //file_writeln(fp, std::string("int retval;"));
            file_writeln(fp, std::string("\toutpro.read_array(") + param.name + ", " + param.name + "_len);");
            //file_writeln(fp, std::string("outpro.read_int(retval);"));
            //file_writeln(fp, std::string("return retval;"));
        }
        else if (param.type == "int"){
            file_writeln(fp, std::string("\toutpro.read_int(") + param.name + ");");
        }
        else if (param.type == "double"){
            file_writeln(fp, std::string("\toutpro.read_double(") + param.name + ");");
        }
        else if (param.type == "float"){
            file_writeln(fp, std::string("\toutpro.read_float(") + param.name + ");");
        }
        else if (param.type == "String"){
            file_writeln(fp, std::string("\tint ") + param.name + "_len_1;");
            file_writeln(fp, std::string("\toutpro.read_string(") + param.name +"_len_1, " + param.name + ");");
        }
    }
    if(return_type == "int"){
        file_writeln(fp, std::string("\tint retval;"));
        file_writeln(fp, std::string("\toutpro.read_int(retval);"));
        file_writeln(fp, std::string("\treturn retval;"));
    }
    else if(return_type == "double"){
        file_writeln(fp, std::string("\tdouble retval;"));
        file_writeln(fp, std::string("\toutpro.read_double(retval);"));
        file_writeln(fp, std::string("\treturn retval;"));
    }
    else if(return_type == "float"){
        file_writeln(fp, std::string("\tfloat retval;"));
        file_writeln(fp, std::string("\toutpro.read_float(retval);"));
        file_writeln(fp, std::string("\treturn retval;"));
    }
    file_writeln(fp, "}\n");
}

static void generate_common_head(const program_t &program,
        const char *filename) {

    FILE *fp = fopen(filename, "w");

    file_writeln(fp, std::string("#ifndef __" + program.name + "_h__"));
    file_writeln(fp, std::string("#define __" + program.name + "_h__"));

    /* generate macros */
    file_writeln(fp, "");
    file_writeln(fp, std::string("#define RPC_ID ") + num_to_str(program.id));
    file_writeln(fp, std::string("#define RPC_NAME \"") + program.name + "\"");
    file_writeln(fp, std::string("#define RPC_VERSION \"") + program.version + "\"");
    file_writeln(fp, std::string("#define DS_IP \"") + program.ds_ip + "\"");
    file_writeln(fp, std::string("#define DS_PORT ") + num_to_str(program.ds_port));

    for (int i = 0; i < program.procedures.size(); ++i) {
        const procedure_t &procedure = program.procedures[i];
        FILE_WRITELN(fp, "#define id_%s %d", procedure.name.c_str(), procedure.id);
    }
    file_writeln(fp, "");

    /* generate procudures */
    for (int i = 0; i < program.procedures.size(); ++i) {
        const procedure_t &procedure = program.procedures[i];

        char line[2048] = { 0 };
        int offsize = sprintf(line, "%s %s(", procedure.rettype.c_str(), procedure.name.c_str());

        for (int j = 0; j < procedure.params.size(); ++j) {
            const param_t &param = procedure.params[j];
            std::string param_str = generate_param(param);
            if (j != procedure.params.size() - 1) {
                offsize += sprintf(line + offsize, "%s, ", param_str.c_str());
            } else {
                offsize += sprintf(line + offsize, "%s", param_str.c_str());
            }
        }
        offsize += sprintf(line + offsize, ")");
        file_write(fp, line);
        file_writeln(fp, ";\n");
    }

    /* finished */
    file_writeln(fp, "#endif");
    fclose(fp);
}

/**
 * @brief generate client stub
 *
 * @param xml_filename
 * @param filename
 */

static void generate_client_stub(const program_t &program,
                                 const char *filename) {
    
    FILE *fp = fopen(filename, "w");
//    ezxml_t root = ezxml_parse_file(xml_filename);
    
    file_writeln(fp, std::string("#include \"") + program.name + ".h\"");
    file_writeln(fp, "#include <assert.h>");
    file_writeln(fp, "#include <stdlib.h>");
    file_writeln(fp, "#include <stdio.h>");
    file_writeln(fp, "#include \"rpc_log.h\"");
    file_writeln(fp, "#include \"rpc_http.h\"");
    file_writeln(fp, "#include \"basic_proto.h\"");
    file_writeln(fp, "#include \"template.h\"");
    file_writeln(fp, "#include \"rpc_common.h\"");
    file_writeln(fp, "");
    /* generate procudures */
    for (int i = 0; i < program.procedures.size(); ++i){
        const procedure_t &procedure = program.procedures[i];
        const int ID = procedure.id;
        const char *name = procedure.name.c_str();
        const char *ret_type = procedure.rettype.c_str();
        char line[2048] = { 0 };
        int offsize = sprintf(line, "%s %s(", ret_type, name);
        for (int j = 0; j < procedure.params.size(); ++j){
            const param_t &param = procedure.params[j];
            std::string param_str = generate_param(param);
            if (j != procedure.params.size() - 1) {
                offsize += sprintf(line + offsize, "%s, ", param_str.c_str());
            } else {
                offsize += sprintf(line + offsize, "%s", param_str.c_str());
            }
        }
        offsize += sprintf(line + offsize, ")");
        file_write(fp, line);
        generate_client_content_stub(fp, procedure.name, name, ret_type, procedure.params);
    }
    fclose(fp);
}

std::string replace(const std::string &src, 
        const std::string &from, const std::string &to) {

    size_t pos = 0;
    std::string str(src);

    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

static void gen_svr_callee(const program_t &program,
        const char *filename) {

    FILE *fp = fopen(filename, "w");

    FILE_WRITELN(fp, "#include \"%s.h\"\n", program.name.c_str());

    /* generate procudures */
    for (int i = 0; i < program.procedures.size(); ++i) {
        const procedure_t &procedure = program.procedures[i];

        char line[2048] = { 0 };
        int offsize = sprintf(line, "%s %s(", procedure.rettype.c_str(), procedure.name.c_str());

        for (int j = 0; j < procedure.params.size(); ++j) {
            const param_t &param = procedure.params[j];
            std::string param_str = generate_param(param);
            if (j != procedure.params.size() - 1) {
                offsize += sprintf(line + offsize, "%s, ", param_str.c_str());
            } else {
                offsize += sprintf(line + offsize, "%s", param_str.c_str());
            }
        }
        offsize += sprintf(line + offsize, ")");
        FILE_WRITE(fp, "%s", line);
        FILE_WRITE(fp, "{\n");
        if (procedure.rettype != "void") {
            FILE_WRITELN(fp, T"return 0;");
        }
        FILE_WRITELN(fp, "}\n");
    }

    fclose(fp);
}

/**
 * @brief generate server main
 *
 * @param xml_filename
 * @param tmpl_filename
 * @param filename
 */
static void gen_svr_main(const program_t &program,
        const char *tmpl_filename, const char *filename) {
    FILE *fp_tmpl = fopen(tmpl_filename, "r");
    FILE *fp_outp = fopen(filename, "w");

    char line[1024] = { 0 };
    while (fgets(line, sizeof(line), fp_tmpl) != NULL) {
        /* output template contents */
        std::string str_line = replace(line, "$id$", num_to_str(program.id));
        str_line = replace(str_line, "$name$", program.name);
        str_line = replace(str_line, "$version$", program.version);
        FILE_WRITE(fp_outp, "%s", str_line.c_str());
    }
    fclose(fp_outp);
    fclose(fp_tmpl);
}

/**
 * @brief generate server stub
 *
 * @param xml_filename
 * @param tmpl_filename
 * @param filename
 */
static void gen_svr_stub_h(const program_t &program,
        const char *tmpl_filename, const char *filename) {

    FILE *fp_tmpl = fopen(tmpl_filename, "r");
    FILE *fp_outp = fopen(filename, "w");

    char line[1024] = { 0 };
    while (fgets(line, sizeof(line), fp_tmpl) != NULL) {
        /* output template contents */
        if (strstr(line, "$stub$")) {
            for (int i = 0; i < program.procedures.size(); ++i) {
                const procedure_t &procedure = program.procedures[i];
                FILE_WRITELN(fp_outp, "void process_%s(const std::string &req_body,", procedure.name.c_str());
                FILE_WRITELN(fp_outp, TT"std::string &rsp_head, std::string &rsp_body) {");
            }
        }
        else {
            std::string str_line = replace(line, "$id$", num_to_str(program.id));
            str_line = replace(str_line, "$name$", program.name);
            str_line = replace(str_line, "$version$", program.version);
            FILE_WRITE(fp_outp, "%s", str_line.c_str());
        }
    }
    fclose(fp_outp);
    fclose(fp_tmpl);
}

static void generate_stub_makefile(const program_t &program, const char *tmpl_filename, const char *filename){
    FILE *fp_tmpl = fopen(tmpl_filename, "r");
    FILE *fp_outp = fopen(filename, "w");
    char line[2048] = {0};
    while(fgets(line, sizeof(line), fp_tmpl) !=NULL){
        std::string str_line = replace(line, "$name$", program.name);
        FILE_WRITE(fp_outp, "%s", str_line.c_str());
    }
    fclose(fp_outp);
    fclose(fp_tmpl);
}

/**
 * @brief generate server stub
 *
 * @param xml_filename
 * @param tmpl_filename
 * @param filename
 */

static void gen_svr_stub_cpp(const program_t &program,
        const char *tmpl_filename, const char *filename) {

    FILE *fp_tmpl = fopen(tmpl_filename, "r");
    FILE *fp_outp = fopen(filename, "w");

    char line[1024] = { 0 };
    while (fgets(line, sizeof(line), fp_tmpl) != NULL) {
        /* dispatch http requests */
        if (strstr(line, "$dispatch$")) {
            for (int i = 0; i < program.procedures.size(); ++i) {
                const procedure_t &procedure = program.procedures[i];
                FILE_WRITELN(fp_outp, TTT"case id_%s:", procedure.name.c_str());
                FILE_WRITELN(fp_outp, TTTT"process_%s(req_body, rsp_head, rsp_body);", procedure.name.c_str());
                FILE_WRITELN(fp_outp, TTTT"break;");
            }
        } 
        /* generate server stub */
        else if (strstr(line, "$stub$")) {
            for (int i = 0; i < program.procedures.size(); ++i) {
                const procedure_t &procedure = program.procedures[i];
                FILE_WRITELN(fp_outp, "void %s_event::process_%s(const std::string &req_body,", program.name.c_str(), procedure.name.c_str());
                FILE_WRITELN(fp_outp, TT"std::string &rsp_head, std::string &rsp_body) {");

                FILE_WRITELN(fp_outp, "");
                FILE_WRITELN(fp_outp, T"basic_proto proto_in(req_body.data(), req_body.size());");
                FILE_WRITELN(fp_outp, T"basic_proto proto_out;");
                FILE_WRITELN(fp_outp, "");

                /* generate unmarchalling codes */
                for (int j = 0; j < procedure.params.size(); ++j) {
                    FILE_WRITELN(fp_outp, T"%s", gen_svr_param_decl(procedure.params[j]).c_str());
                }
                FILE_WRITELN(fp_outp, "");
                for (int j = 0; j < procedure.params.size(); ++j) {
                    FILE_WRITELN(fp_outp, T"%s", gen_svr_param_unmarshalling(procedure.params[j]).c_str());
                }
                
                /* callee */
                char line[2048] = { 0 };
                int offsize = 0;
                if (procedure.rettype == "void") {
                    offsize = sprintf(line, T"%s(", procedure.name.c_str());
                } else {
                    offsize = sprintf(line, T"%s ret_val = %s(", procedure.rettype.c_str(), procedure.name.c_str());
                }
                for (int j = 0; j < procedure.params.size(); ++j) {
                    std::string param_str = gen_svr_param(procedure.params[j]);
                    if (j > 0) {
                        offsize += sprintf(line + offsize, ", ");
                    }
                    offsize += sprintf(line + offsize, "%s", param_str.c_str());
                }
                offsize += sprintf(line + offsize, ");");
                FILE_WRITELN(fp_outp, "%s", line);

                /* generate marchalling codes */
                for (int j = 0; j < procedure.params.size(); ++j) {
                    FILE_WRITELN(fp_outp, T"%s", gen_svr_param_marshalling(procedure.params[j]).c_str());
                }
                if (procedure.rettype != "void") {
                    FILE_WRITELN(fp_outp, "");
                    FILE_WRITELN(fp_outp, T"proto_out.add_%s(ret_val);", procedure.rettype.c_str());
                }
                FILE_WRITELN(fp_outp, "");

                FILE_WRITELN(fp_outp, T"rsp_head = gen_http_head(\"200 OK\", proto_out.get_buf_len());");
                FILE_WRITELN(fp_outp, T"rsp_body.assign(proto_out.get_buf(), proto_out.get_buf_len());");
                FILE_WRITELN(fp_outp, "}\n");
            }
        } 
        /* output template contents */
        else {
            std::string str_line = replace(line, "$id$", num_to_str(program.id));
            str_line = replace(str_line, "$name$", program.name);
            str_line = replace(str_line, "$version$", program.version);
            FILE_WRITE(fp_outp, "%s", str_line.c_str());
        }
    }
    fclose(fp_outp);
    fclose(fp_tmpl);
}

void init(const char *xml_filename, program_t &program) {
    ezxml_t root = ezxml_parse_file(xml_filename);

    /* program information */
    program.id      = atoi(ezxml_child(root, "id")->txt);
    program.name    = ezxml_child(root, "name")->txt;
    program.version = ezxml_child(root, "version")->txt;
    program.ds_ip   = ezxml_child(root, "directory_server_ip")->txt;
    program.ds_port = atoi(ezxml_child(root, "directory_server_port")->txt);

    std::vector<procedure_t> &procedures = program.procedures;

    /* procudures' information */
    for (ezxml_t child = ezxml_child(root, "procedure"); child != NULL; child = child->next) {
        procedure_t procedure;
        procedure.id      = atoi(ezxml_child(child, "id")->txt);
        procedure.name    = ezxml_child(child, "name")->txt;
        procedure.rettype = ezxml_child(child, "return")->txt;

        /* params' information */
        std::vector<param_t> &params = procedure.params;
        for (ezxml_t sub_child = ezxml_child(child, "param"); sub_child != NULL; sub_child = sub_child->next) {
            param_t param;
            param.index = atoi(ezxml_child(sub_child, "index")->txt);
            param.name  = ezxml_child(sub_child, "name")->txt;
            param.type  = ezxml_child(sub_child, "type")->txt;
            params.push_back(param);
        }
        std::sort(params.begin(), params.end(), comp_index);
        procedures.push_back(procedure);
    }
}

int main(int argc, char *argv[]) {
    /* command line options */
    const char *target = NULL;
    const char *path = NULL;
    const char *xml_file = NULL;
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h'},
            { "target", required_argument, 0, 't'},
            { "path", required_argument, 0, 'p'},
            { "xml", required_argument, 0, 'x'},
            { 0, 0, 0, 0 }
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "ht:p:x:", long_options, &option_index);
        if (-1 == c) {
            break;
        }
        switch (c) {
            case 'h':
                usage(argc, argv);
                exit(0);
            case 't':
                target = optarg;
                break;
            case 'p':
                path = optarg;
                break;
            case 'x':
                xml_file = optarg;
                break;
            default:
                usage(argc, argv);
                exit(0);
        }
    }

    if (!target || !path || !xml_file) {
        usage(argc, argv);
        exit(0);
    }
    if (strcmp(target, "client_stub") != 0 && strcmp(target, "server_stub") != 0) {
        usage(argc, argv);
        exit(0);
    }

    program_t program;
    init(xml_file, program);

    if (strcmp(target, "client_stub") == 0) {
        std::string prefix(path);
        std::string mkf_path(path);
        mkf_path += "/Makefile";
        prefix += "/" + program.name;
        generate_common_head(program, std::string(prefix + ".h").c_str());
        generate_client_stub(program, std::string(prefix + "_client_stub.cpp").c_str());
        gen_svr_main(program, "conf/main_cli.tmpl.cpp", (std::string(path) + "/main_demo.cpp").c_str());
        generate_stub_makefile(program, "conf/make_cli.tmpl", mkf_path.c_str());
    }
    else if (strcmp(target, "server_stub") == 0){
        std::string prefix(path);
        std::string mkf_path(path);
        mkf_path += "/Makefile";
        prefix += "/" + program.name;
        generate_common_head(program, std::string(prefix + ".h").c_str());
        gen_svr_stub_h(program, "conf/svr.tmpl.h", std::string(prefix + "_svr.h").c_str());
        gen_svr_stub_cpp(program, "conf/svr.tmpl.cpp", std::string(prefix + "_svr.cpp").c_str());
        gen_svr_callee(program, std::string(prefix + "_svr_callee.cpp").c_str());
        gen_svr_main(program, "conf/main_svr.tmpl.cpp", (std::string(path) + "/main_" + program.name + "_svr.cpp").c_str());
        generate_stub_makefile(program, "conf/make_svr.tmpl", mkf_path.c_str());
    }

    exit(0);
}
