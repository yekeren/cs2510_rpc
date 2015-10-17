#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "ezxml.h"
#include "rpc_log.h"

struct param_t {
    int index;
    std::string name;
    std::string type;
};

static void usage(int argc, char *argv[]) {
    printf("Usage: %s idl_filename output_filename\n", argv[0]);
}

static void file_writeln(FILE *fp, const std::string &line) {
    //puts((line).c_str());
    fputs((line + "\n").c_str(), fp);
}

static void file_write(FILE *fp, const std::string &line) {
    fprintf(fp, "%s", (line).c_str());
    //fputs((line + "\n").c_str(), fp);
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
static void generate_common_head(const char *xml_filename,
        const char *filename) {
    FILE *fp = fopen(filename, "w");
    ezxml_t root = ezxml_parse_file(xml_filename);

    file_writeln(fp, "#ifndef __RPC_COMMON_H__");
    file_writeln(fp, "#define __RPC_COMMON_H__");

    /* generate macros */
    const char *id = ezxml_child(root, "id")->txt;
    const char *name = ezxml_child(root, "name")->txt;
    const char *version = ezxml_child(root, "version")->txt;

    file_writeln(fp, "");
    file_writeln(fp, std::string("#define RPC_ID ") + id);
    file_writeln(fp, std::string("#define RPC_NAME \"") + name + "\"");
    file_writeln(fp, std::string("#define RPC_VERSION \"") + version + "\"");
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

    /* finished */
    file_writeln(fp, "#endif");

    ezxml_free(root);
    fclose(fp);
}

/**
 * @brief generate client stub
 *
 * @param xml_filename
 * @param filename
 */
static void generate_client_stub(const char *xml_filename,
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
        file_writeln(fp, " { \n");
        file_writeln(fp, "}\n");
    }

    ezxml_free(root);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (2 != argc) {
        usage(argc, argv);
        exit(0);
    }

    generate_common_head(argv[1], "test.h");
    generate_client_stub(argv[1], "test.cpp");

    exit(0);
}
