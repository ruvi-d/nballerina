// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | grep -e "RETVAL=16"

int _val = 0;
public function main(){
    _val = 8;
}
