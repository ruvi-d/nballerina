// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

public function print_str(string val) = external;

public function main() {
    string str = "Hello World!";
    print_str("RESULT=");
    print_str(str);
}
// CHECK: RESULT=Hello World!
