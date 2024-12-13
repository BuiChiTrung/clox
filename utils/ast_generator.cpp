#include "helper.hpp"
#include <fstream>
#include <iostream>
#include <vector>

std::ofstream out("../ast/expr_gen.hpp");

void define_preprocessor() { out << "#include \"../token.hpp\"\n\n"; }

void define_class_fields(std::vector<std::string> class_fields) {
    out << "  private:\n";
    for (std::string field : class_fields) {
        field = strip(field);
        out << "\t" << field << ";\n";
    }
}

void define_classes(std::string base_class,
                    std::vector<std::string> sub_classes) {
    out << "class Expr {}\n\n";

    for (std::string sub_class : sub_classes) {
        std::vector<std::string> parts = split_string(sub_class, '#');
        if (parts.size() != 2) {
            std::cout << "Invalid subclass string" << std::endl;
            exit(1);
        }

        std::string class_name = strip(parts[0]);
        out << "class " << class_name << " : " << "public " << base_class
            << "{\n";
        std::vector<std::string> class_fields =
            split_string(strip(parts[1]), ',');
        define_class_fields(class_fields);
        out << "};\n\n";
    }
}

int main() {
    define_preprocessor();

    std::string base_class = "Expr";
    std::vector<std::string> sub_classes = {
        "Binary# Expr left, Token op, Expr right", "Grouping# Expr expression",
        "Literal# std::any value", "Unary# Token op, Expr right"};
    define_classes(base_class, sub_classes);
}