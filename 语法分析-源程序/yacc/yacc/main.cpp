#include <ctype.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <map>
#include <vector>
using std::endl;
using std::cout;
using std::cin;

#define SUCCESS 1
#define FAIL 0


// ����������
static char reserveWord[32][20]{
 "auto",       "break",    "case",    "char",        "const",      "continue",
 "default",   "do",       "double",   "else",        "enum",       "extern",
 "float",      "for",      "goto",     "if",          "int",        "long",
 "register",   "return",   "short",    "signed",     "sizeof",     "static",
 "struct",     "switch",   "typedef",  "union",      "unsigned",   "void",
 "volatile",    "while"
};

// �ָ�������
static char spliterWord[14][10] = {
       ";", "(", ")", "^", ",",
       "\"", "\'", "#","[","]",
       "{", "}", "\\", "."
 };

typedef struct{
    std::string term = "";
    int kind = -1;
}Token;

typedef struct{
    std::string leftTerm;
    std::string rightTerm;
}Pattern;

// ���ڴ洢�ʷ��������õ�Token
std::vector<Token> Tokens;

// ���ڴ���﷨������Ҫ�õ��Ĳ���ʽPattern
std::vector<Pattern> Patterns;

// ���ڱ���Tokens[]�ı���
static int index = 0;

// ���ڴ�Ų���ʽ�Ƶ�����
std::vector<int> seq;

inline void setToken(Token&  token, std::string term, int kind){
    token.term = term;
    token.kind = kind;
}

inline void setPattern(Pattern & pattern, std::string leftTerm, std::string rightTerm){
    pattern.leftTerm = leftTerm;
    pattern.rightTerm = rightTerm;
}

// ��ʼ������ʽ����
void initPattern(std::vector<Pattern>& Patterns, Pattern tmp){
    setPattern(tmp, "E", "idE\'");
    Patterns.push_back(tmp);
    setPattern(tmp, "E", "numE\'");
    Patterns.push_back(tmp);
    setPattern(tmp, "E", "(E)E\'");
    Patterns.push_back(tmp);
    setPattern(tmp, "E\'", "+EE\'");
    Patterns.push_back(tmp);
    setPattern(tmp, "E\'", "-EE\'");
    Patterns.push_back(tmp);
    setPattern(tmp, "E\'", "*EE\'");
    Patterns.push_back(tmp);
    setPattern(tmp, "E\'", "/EE\'");
    Patterns.push_back(tmp);
    setPattern(tmp ,"E\'", "e");
    Patterns.push_back(tmp);
}

// ���˵���ע�ͺͶ���ע��
void filterResource(const char r[], const int pProject, char * filtered, int& cnt);

// ƥ�䱣����
int searchRsv(const char* token, const char reserveWord[][20]);

// ƥ��ָ���
int searchSpliter(const char * token, const char spliterWord[][10]);

// ƥ�������
int searchOperator(const char * token, const char operatorWord[][10]);

// ɨ����/�ʷ�������
void Scanner(int & syn, const char r[], char * token, int & pProject);

// ����syn��ֵȷ��token ������
int Analysis(int syn);

// ���ôʷ�����ģ��
void yylex();

void Parse_E();
void Parse_E1();

// �����﷨����ģ��
void yyyacc();

int main()
{
    cout << "�����ӡ�ʷ����������" << endl << endl;
    yylex();
    int cnt = 0;
    cout << endl << endl;
    cout << "�����ӡ�﷨�������̣�" << endl << endl;
    yyyacc();
    return 0;
}

void filterResource(const char r[], const int pProject, char * filtered, int& cnt){
    char tempString[10000];
    for(int i = 0; i <= pProject; ++ i){
        if(r[i] == '/' && r[i + 1] == '/'){             // ���ֵ���ע�ͣ�����ע������
            while(r[i] != '\n'){
                ++ i;                                   // �������ɨ�裬ֱ�����ֻ��з���
            }
        }

        if(r[i] == '/' && r[i+1] == '*'){               // ���ֶ���ע�ͣ�����ע������
            i += 2;
            while(r[i] != '*' && r[i + 1] != '/'){
                    ++ i;
                if(r[i] == '$'){
                    cout << "����ע��ȱ�� */�� �������" << endl;
                    exit(0);
                }
            }
            i += 2;
        }

        if(r[i] != '\r' && r[i] != '\n' && r[i] != '\t'){
            tempString[cnt++] = r[i];                   // ����Ч�ַ��������µ��ַ�����
        }
    }

    tempString[cnt] = '\0';
    strcpy(filtered, tempString);
}

int searchRsv(const char * token, const char reserveWord[][20]){
    for(int i = 0; i < 32; ++ i){
        if(strcmp(token, reserveWord[i]) == 0){
            return i + 1;
        }
    }
    return -1;
}

int searchSpliter(const char * token, const char spliterWord[][10]){
    for(int i = 0; i < 14; ++ i){
        if(strcmp(token, spliterWord[i]) == 0){
            return i + 53;
        }
    }
    return -1;
}

int searchOperator(const char * token, const char operatorWord[][10]){
    for(int i = 0; i < 20; ++ i){
        if(strcmp(token, spliterWord[i]) == 0){
            return i + 33;
        }
    }
    return -1;
}

void Scanner(int & syn, const char r[], char * token, int & pProject){
    //����DFA��״̬ת��ͼ���
    int i;
    int j = 0;      //  ����token ��ָʾ��
    while(r[pProject] == ' '){
        pProject ++;
    }

    for (i = 0; i<20; i++)
    {//ÿ���ռ�ǰ������
        token[i] = '\0';
    }

    // ��ͷΪ��ĸ���»���
    if(isalpha(r[pProject]) || r[pProject] == '_'){
        token[j++] = r[pProject];          // �ռ�
        pProject++;                             // ����
        while(isalnum(r[pProject]) || r[pProject] == '_'){
            // �����ĸ���»���
            token[j++] = r[pProject];      // �ռ�
            pProject++;                         // ����
        }

        token[j] = '\0';

        syn =  searchRsv(token, reserveWord);

        // ���ǹؼ��ּ�Ϊ��ʶ��
        if(syn == -1){
            syn = 100;
        }
        return;
    }

    // ���ַ�Ϊ����
    else if(isdigit(r[pProject])){
        token[j++] = r[pProject];               // �ռ�
        pProject ++;                            // ����
        while(isdigit(r[pProject]) || r[pProject] == '.'){
            token[j++] = r[pProject];           // �ռ�
            pProject ++;                        // ����
        }

        token[j] = '\0';

        // �ִ����״̬
        if(isdigit(token[j-1])){
            syn = 99;
        }

        return;
    }

    // + | ++
    else if(r[pProject] == '+'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '+'){
            token[j++] = r[pProject];
            syn = 38;
        }

        else{
            syn = 33;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    // - | --
    else if(r[pProject] == '-'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '-'){
            token[j++] = r[pProject];
            syn = 39;
        }

        else{
            syn = 34;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    //  = | ==
    else if(r[pProject] == '='){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 45;
        }

        else{
            syn = 44;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '<'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '<'){
            token[j++] = r[pProject];
            syn = 51;
        }

        else if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 41;
        }

        else{
            syn = 40;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '>'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '>'){
            token[j++] = r[pProject];
            syn = 52;
        }

        else if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 43;
        }

        else{
            syn = 42;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '!'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '='){
            token[j++] = r[pProject];
            syn = 46;
        }

        else{
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '&'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '&'){
            token[j++] = r[pProject];
            syn = 48;
        }

        else{
            syn = 47;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '|'){
        token[j++] = r[pProject];
        pProject ++;

        if(r[pProject] == '|'){
            token[j++] = r[pProject];
            syn = 50;
        }

        else{
            syn = 49;
            pProject --;
        }

        token[j] = '\0';
        pProject ++;
        return;
    }

    else if(r[pProject] == '*'){

            token[j++] = r[pProject];
            token[j] = '\0';
            pProject ++;
            syn = 35;
            return;
    }

    else if(r[pProject] == '/'){

            token[j++] = r[pProject];
            token[j] = '\0';
            pProject ++;
            syn = 36;
            return;
    }

    else if(r[pProject] == '%'){

            token[j++] = r[pProject];
            token[j] = '\0';
            pProject ++;
            syn = 37;
            return;
    }



    // �ָ���
    else if(r[pProject] == ';' || r[pProject] == '(' || r[pProject] == ')' || r[pProject] == '^' ||
        r[pProject] == ',' || r[pProject] == '\"' || r[pProject] == '\'' || r[pProject] == '#'  ||
        r[pProject] == '['  || r[pProject] == ']' || r[pProject] == '{' || r[pProject] == '}' ||
        r[pProject] == '\\' || r[pProject] == '.'){

        token[j++] = r[pProject];
        pProject ++;
        token[j] = '\0';
        syn = searchSpliter(token, spliterWord);
    }


    else if(r[pProject] == '$'){
        syn = 0;
    }

    else{
        printf("Can't find the symbol %c, the pointer proceed\n", r[pProject]);
        pProject++;
    }
}

void yylex()
{
    // ����Դ�ļ�����ȡ���е�Դ����
    int syn = -1;
    char resourceProject[10000];
    char token[20] = {0};
    int pProject = 0;
    FILE *fp;
    FILE *fp1;
    if((fp = fopen("source.txt", "r")) == NULL){
        cout << "��Դ�ļ�ʧ��" << endl;
        exit(0);
    }
    resourceProject[pProject] = fgetc(fp);
    while(resourceProject[pProject] != EOF){
        pProject++;
        resourceProject[pProject] = fgetc(fp);
    }

    resourceProject[++pProject] = '\0';
    fclose(fp);

    cout << "\nԴ����Ϊ��\n";
    cout << resourceProject << endl;

    if((fp1 = fopen("target.txt", "w+"))== NULL){
        cout << "��Ŀ���ļ�ʧ��" << endl;
        exit(0);
    }

    char filtered[10000] = {0};
    int cnt = 0;

    filterResource(resourceProject, pProject, filtered, cnt);

    cout << "\n���˺��Դ����Ϊ��" << endl;
    cout << filtered << endl;
    fprintf(fp1, filtered);
    cout << "�ַ�����" << cnt << endl << endl;
    fprintf(fp1, "\n�ַ�����%d\n\n", cnt);

    pProject = 0;

    while(syn != 0){
        int kind = -1;
        int syn = -1;
        Scanner(syn, filtered, token, pProject);

        if(syn == -1){
            continue;
        }

        if(syn == 0){
            break;
        }

        if(syn == 100){
            // ��ʶ��
            kind = 2;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn >=1 && syn <= 32){
            // �ؼ���
            kind = 1;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn == 99){
            // ����
            kind = 3;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn >= 33 && syn <= 52){
            // �����
            kind = 4;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }

        if(syn >= 53 && syn <= 66){
            // �ָ���
            kind = 5;
            printf("( \" %s\" , %d )\n", token, kind);
            fprintf(fp1, "( \" %s\" , %d )\n", token, kind);
            Token current;
            setToken(current, token, kind);
            Tokens.push_back(current);
        }
    }
    fclose(fp1);
}

void yyyacc(){
    Pattern temp;
    initPattern(Patterns,temp);
//    cout << "\n\n����ʽ������ʼ���ɹ�" << endl;
//    cout << Patterns.size() << endl;
//    cout << endl << endl;
    cout << "\n\n�﷨������ʹ�õ����ķ���" << endl;
    for(long long unsigned int i = 0; i < Patterns.size(); ++ i){
        cout << Patterns[i].leftTerm << "-->" << Patterns[i].rightTerm << endl;
    }
//
//    cout << "����ݹ��½������ӳ���" << endl;
    Parse_E();
    cout << "\n\nsyntax analysis has completed!!!" << endl;
    cout << "\n\n�����ӡ���ҹ�Լ���̣�" << endl;
    for(int j = 0; j < seq.size(); j ++){
        cout << Patterns[seq[j]].leftTerm << "-->" << Patterns[seq[j]].rightTerm << endl;
    }

    cout << "\n\n�����ӡ�����Ƶ����̣�" << endl;
    for(int j = seq.size() - 1; j >= 0; j--){
        cout << Patterns[seq[j]].leftTerm << "-->" << Patterns[seq[j]].rightTerm << endl;
    }
}

void Parse_E(){
    Token token = Tokens[index++];
//    cout << "\n\n�ɹ���ȡtoken[]" << endl;
//    cout << "\n\ncurrent token:" << token.term << endl;

    if(token.term == "$"){
        return;
    }

    else if(token.kind == 2){
//        cout << "\n\nʹ�ò���ʽ0" << endl;
        Parse_E1();
        seq.push_back(0);
    }

    else if(token.kind == 3){
//        cout << "\n\nʹ�ò���ʽ1" << endl;
        Parse_E1();
        seq.push_back(1);
    }

    else if(token.term == "("){
//        cout << "\n\nʹ�ò���ʽ2" << endl;
        Parse_E();

        if(token.term == ")"){
            cout << "��ǰtoken: " << token.term << endl;
            system("pause");
            index ++;
            Token token = Tokens[index];
            cout << "��ǰtoken: " << token.term << endl;
            system("pause");
            Parse_E1();
            seq.push_back(2);
        }

        else{
//            cout << "\n\nexpected a \')\', but got a " << token.term << endl;
        }
    }

    else{
        cout << "\n\nNO Patterns can match the token, the program exits with 0" << endl;
        exit(0);
    }
}

void Parse_E1(){
    Token token = Tokens[index++];
//    cout << "\n\n�ɹ���ȡtoken[]" << endl;
//    cout << "\n\ncurrent token:" << token.term << endl;

    if(token.term == "$"){
        return;
    }

    else if(token.term == "+"){
//        cout << "\n\nʹ�ò���ʽ3" << endl;
        Parse_E();
        Parse_E1();
        seq.push_back(3);
    }

    else if(token.term == "-"){
//        cout << "\n\nʹ�ò���ʽ4" << endl;
        Parse_E();
        Parse_E1();
        seq.push_back(4);
    }

    else if(token.term == "*"){
//        cout << "\n\nʹ�ò���ʽ5" << endl;
        Parse_E();
        Parse_E1();
        seq.push_back(5);
    }

    else if(token.term == "/"){
//        cout << "\n\nʹ�ò���ʽ6" << endl;
        Parse_E();
        Parse_E1();
        seq.push_back(6);
    }

    else if(token.term == ""){
//        cout << "\n\nʹ�ò���ʽ7" << endl;
        seq.push_back(7);
    }

    else if(token.term == ")"){

    }

    else{
        cout << "\n\nNo Patterns can match the token, the program exits with 0" << endl;
        exit(0);
    }
}
