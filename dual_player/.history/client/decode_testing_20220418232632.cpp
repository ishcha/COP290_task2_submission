#include<stdio.h>
#include<iostream>
#include<cstring>

void decode(const char* data) {
    if (strlen(data) <= 4) return;
    int i = 0,  c = 0;
    while (data[i] != '\0') { // something remains to be seen in the packet
    printf("packet decode while loop \n");
    printf(data[i])
        switch (data[i]) {
            case 'p': // update player 2
                {i += 4; c = 0;
                while (isdigit(data[i])) {
                    c = 10*c + (data[i]-'0');
                    i++;
                }
                i += 2;
                break;}
            case 'P':
                {i += 2; c = 0;
                while (data[i] != '\n') {
                    switch (data[i]) {
                        case 'x':
                            i+= 2; c=0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            i++;
                            break;
                        case 'y':
                            i+= 2; c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            i++;
                            break;
                        case 'e':
                            i+= 2; c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            i++;
                            break;
                        case 'h':
                            i+= 2;
                            break;
                        default:
                            i++; break;
                    }
                }
                break;}
            case 'D':
                {i += 2; c = 0; int index;
                while (data[i] != '\n') {
                    index = data[i]-'0'; // assuming that the indices are 1 digit long
                    i += 2;
                    while (!isdigit(data[i])) {
                        switch (data[i]) {
                            case 'm':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                i++;
                                break;
                            case 'p':
                                i += 2;
                                i+=2;
                                break;
                            case 'd':
                                i += 2;
                                i+=2;
                                break;
                            case 'x':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                i++;
                                break;
                            case 'y':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                i++;
                                break;
                            default:
                                i++;
                                break;
                        }
                    }
                }  
                i++;             
                break;}
            case 'G':
                {i += 2; c = 0; int index;
                while (data[i] != '\n') {
                    index = data[i]-'0'; // assuming that the indices are 1 digit long
                    i += 2;
                    while (!isdigit(data[i])) {
                        switch (data[i]) {
                            case 'm':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                i++;
                                break;
                            case 'p':
                                i += 2;
                                i+=2;
                                break;
                            case 'd':
                                i += 2;
                                i+=2;
                                break;
                            case 'x':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                i++;
                                break;
                            case 'y':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                i++;
                                break;
                            default:
                                i++;
                                break;
                        }
                    }
                }  
                i++;             
                break;}
            default:
                i++;
                break;
        }  
    }     
}

int main() {

    const char* input = "D 0 m 1 p 1 d 1 x 17 \n G 3 m 1 p 1 d 0 y 6 \n ";

decode(input);

return 0;
}
