#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "ezxml.h"
#include "rpc_log.h"

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
    printf("Usage: %s idl_filename output_filename\n", argv[0]);
}

static void file_writeln(FILE *fp, const std::string &line) {
    //puts((line).c_str());
    fputs((line + "\n").c_str(), fp);
}

static void file_write(FILE *fp, const std::string &line) {
    fprintf(fp, "%s", (line).c_str());
    //fprintf(stdout, "%s", (line).c_str());
}

static std::string generate_param(const param_t &param) {
    if (param.type == "Matrix") {
        return "int *" + param.name + ", int " + param.name + "_row, int " + param.name + "_col";
    } else if (param.type == "Array" ){
        return "int " + param.name + "_len, int *" + param.name;
    } else {
        return param.type + " " + param.name;
    }
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

static void gen_req_rsp(FILE *fp, const char *name){
    std::string nameStr = name;
    file_writeln(fp, std::string("std::string req_head = gen_http_head(") + "\"" + "/" + nameStr + "\"" + ", svr_inst.ip, inpro.get_buf_len());");
    file_writeln(fp, std::string("std::string req_body(inpro.get_buf(), inpro.get_buf_len());"));
    file_writeln(fp, std::string("std::string rsp_head, rsp_body;"));
    file_writeln(fp, std::string("http_talk(svr_inst.ip, svr_inst.port, req_head, req_body, rsp_head, rsp_body);"));
    file_writeln(fp, std::string("basic_proto outpro(rsp_body.data(), rsp_body.size());"));
}

static void generate_client_content_stub(FILE *fp, const char *name, const char *ret_type, std::vector<param_t> params){
    file_writeln(fp, " { \n");
    file_writeln(fp, "basic_proto inpro;");
    std::string nameStr = name;
    for (int i = 0; i < params.size(); ++i){
        param_t &param = params[i];
        if (param.type == "Matrix"){
            file_write(fp, std::string("inpro.add_matrix("));
            file_write(fp, param.name + ", " + param.name + "_row," + param.name + "_col");
            file_writeln(fp,std::string(");"));
        }
        else if (param.type == "Array"){
            file_write(fp, std::string("inpro.add_array("));
            file_writeln(fp, param.name + ", " + param.name + "_len);");
        }
        else if (param.type == "int"){
            file_write(fp, std::string("inpro.add_int("));
            file_writeln(fp, param.name + ");");
        }
        else if (param.type == "double"){
            file_write(fp, std::string("inpro.add_double("));
            file_writeln(fp, param.name + ");");
        }
        else if (param.type == "float"){
            file_write(fp, std::string("inpro.add_float("));
            file_writeln(fp, param.name + ");");
        }
        else if (param.type == "char*"){
            file_writeln(fp, std::string("std::string ") + param.name +"_str = " + param.name + ";");
            file_writeln(fp, std::string("inpro.add_string(" + param.name +"_str" + ".size(), " + param.name +"_str" + ".data());"));
        }
    }
    gen_req_rsp(fp, nameStr.c_str());
    for (int i = 0; i < params.size(); ++i){
        param_t &param = params[i];
        if (param.type == "Matrix"){
            file_writeln(fp, std::string("int ") + "*" + param.name + "_bak;");
            file_writeln(fp, std::string("outpro.read_matrix(") + param.name + "_bak, " + param.name + "_row, " + param.name + "_col);");
            file_writeln(fp, std::string("memcpy(") + param.name + ", " + param.name + "_bak, " + "sizeof(int) * " + param.name + "_row" + " * " + param.name + "_col);");
        }
        else if (param.type == "Array"){
            //file_writeln(fp, std::string("int retval;"));
            file_writeln(fp, std::string("outpro.read_array(") + param.name + ", " + param.name + "_len);");
            //file_writeln(fp, std::string("outpro.read_int(retval);"));
            //file_writeln(fp, std::string("return retval;"));
        }
        else if (param.type == "int"){
            file_writeln(fp, std::string("outpro.read_int(") + param.name + ");");
        }
        else if (param.type == "double"){
            file_writeln(fp, std::string("outpro.read_double(") + param.name + ");");
        }
        else if (param.type == "float"){
            file_writeln(fp, std::string("outpro.read_float(") + param.name + ");");
        }
        else if (param.type == "char*"){
            file_writeln(fp, std::string("int ") + param.name + "_len;");
            file_writeln(fp, std::string("outpro.read_string(") + param.name +"_len, " + param.name + ");");
        }
    }
    std::string return_type = ret_type;
    if(return_type == "int"){
        file_writeln(fp, std::string("int retval;"));
        file_writeln(fp, std::string("outpro.read_int(retval);"));
        file_writeln(fp, std::string("return retval;"));
    }
    else if(return_type == "double"){
        file_writeln(fp, std::string("double retval;"));
        file_writeln(fp, std::string("outpro.read_double(retval);"));
        file_writeln(fp, std::string("return retval;"));
    }
    else if(return_type == "float"){
        file_writeln(fp, std::string("float retval;"));
        file_writeln(fp, std::string("outpro.read_float(retval);"));
        file_writeln(fp, std::string("return retval;"));
    }
    file_writeln(fp, "}\n");
}

static void generate_common_head(const program_t &program,
        const char *filename) {

    FILE *fp = fopen(filename, "w");

    file_writeln(fp, "#ifndef __RPC_COMMON_H__");
    file_writeln(fp, "#define __RPC_COMMON_H__");

    /* generate macros */
    file_writeln(fp, "");
    file_writeln(fp, std::string("#define RPC_ID ") + num_to_str(program.id));
    file_writeln(fp, std::string("#define RPC_NAME \"") + program.name + "\"");
    file_writeln(fp, std::string("#define RPC_VERSION \"") + program.version + "\"");
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
    
    file_writeln(fp, "#include \"test.h\"");
    file_writeln(fp, "");
    /* request server */
    file_writeln(fp, std::string("svr_inst_t svr_inst;"));
    //file_writeln(fp, std::string("int ret_val = get_and_verify_svr(") + program.ds_ip +", " + program.ds_port + ", " + program.id +", " + program.version + ", " + "svr_inst);");
    file_writeln(fp, std::string("RPC_INFO(")+ "\"" + "server verified, id=%d, version=%s, ip=%s, port=%u" + ", " + "svr_inst.id, svr_inst.version.c_str(), svr_inst.ip.c_str(), svr_inst,port);");
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
        generate_client_content_stub(fp, name, ret_type, procedure.params);
    }
    fclose(fp);
}

class svr_stub_gen {
    public:
        svr_stub_gen() {
        }
        virtual ~svr_stub_gen() {
        }
    public:
        void set_id(int id) {
            m_id = id;
        }
        void set_version(const std::string &version) {
            m_version = version;
        }
        void set_name(const std::string &name) {
            m_name = name;
        }

    private:
        std::string m_name;
        std::string m_version;
        int m_id;
};

/**
 * @brief generate server stub
 *
 * @param xml_filename
 * @param filename
 */

static void generate_server_stub(const char *xml_filename,
        const char *filename) {

    FILE *fp = fopen(filename, "w");
    ezxml_t root = ezxml_parse_file(xml_filename);

    file_writeln(fp, "#include \"test.h\"");
    file_writeln(fp, "");

    /* generate procudures */
    for (ezxml_t child = ezxml_child(root, "procedure"); child != NULL; child = child->next) {
        const char *name = ezxml_child(child, "name")->txt;
        const char *ret_type = ezxml_child(child, "return")->txt;
        
        char line[2048] = { 0 };
        int offsize = sprintf(line, "%s %s(", ret_type, name);

        std::vector<param_t> params;
        for (ezxml_t sub_child = ezxml_child(child, "param"); sub_child != NULL; sub_child = sub_child->next) {
            param_t param;
            param.index = atoi(ezxml_child(sub_child, "index")->txt);
            param.type = ezxml_child(sub_child, "type")->txt;
            param.name = ezxml_child(sub_child, "name")->txt;
            params.push_back(param);
        }
        std::sort(params.begin(), params.end(), comp_index);
        for (int i = 0; i < params.size(); ++i) {
            param_t &param = params[i];
            std::string param_str = generate_param(param);
            if (i != params.size() - 1) {
                offsize += sprintf(line + offsize, "%s, ", param_str.c_str());
            } else {
                offsize += sprintf(line + offsize, "%s", param_str.c_str());
            }
        }
        offsize += sprintf(line + offsize, ")");
        file_write(fp, line);
        file_writeln(fp, ";\n");
    }

    ezxml_free(root);
    fclose(fp);
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
    if (2 != argc) {
        usage(argc, argv);
        exit(0);
    }

    program_t program;
    init("conf/idl.xml", program);

    generate_common_head(program, "test.h");
    generate_client_stub(program, "test.cpp");
    //generate_server_stub(argv[1], "test_svr.cpp");

    exit(0);
}
