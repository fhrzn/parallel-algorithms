#include<iostream>
using namespace std;


int main(int argc, char *argv[]) {

    char* words = argv[1];    

    int count = 0;
    bool isSeparator = true;

    while(*words) {
        
        if (*words == ' ' || *words == '\t' || *words == '\n') {            
            isSeparator = true;
        }

        if (isSeparator) {
            count++;
            isSeparator = false;
        }

        words++;
    }

    cout << "Total word(s): " << count << endl;
    
    return 0;

}