#include<>

void decode(char* data) {
    if (strlen(data) <= 4) return;
    int i = 0,  c = 0;
    while (data[i] != '\0') { // something remains to be seen in the packet
    printf("packet decode while loop \n");
        switch (data[i]) {
            case 'p': // update player 2
                {i += 4; c = 0;
                while (isdigit(data[i])) {
                    c = 10*c + (data[i]-'0');
                    i++;
                }
                p2.set_energy(c);
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
                            p1.set_x(c);
                            i++;
                            break;
                        case 'y':
                            i+= 2; c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            p1.set_y(c);
                            i++;
                            break;
                        case 'e':
                            i+= 2; c = 0;
                            while (isdigit(data[i])) {
                                c = 10*c + (data[i]-'0');
                                i++;
                            }
                            p1.set_energy(c);
                            i++;
                            break;
                        case 'h':
                            i+= 2;
                            p1.set_has_key();
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
                                dogs[index].set_moves(c);
                                i++;
                                break;
                            case 'p':
                                i += 2;
                                dogs[index].set_disappear(data[i]-'0');
                                i+=2;
                                break;
                            case 'd':
                                i += 2;
                                dogs[index].set_direction(data[i]-'0');
                                i+=2;
                                break;
                            case 'x':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                dogs[index].set_x(c);
                                i++;
                                break;
                            case 'y':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                dogs[index].set_y(c);
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
                                guards[index].set_moves(c);
                                i++;
                                break;
                            case 'p':
                                i += 2;
                                guards[index].set_disappear(data[i]-'0');
                                i+=2;
                                break;
                            case 'd':
                                i += 2;
                                guards[index].set_direction(data[i]-'0');
                                i+=2;
                                break;
                            case 'x':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                guards[index].set_x(c);
                                i++;
                                break;
                            case 'y':
                                i += 2; c = 0;
                                while (isdigit(data[i])) {
                                    c = 10*c + (data[i]-'0');
                                    i++;
                                }
                                guards[index].set_y(c);
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

    char* input = "D 0 m 1 p 1 d 1 x 17 \n G 3 m 1 p 1 d 0 y 6 \n ";

decode(input);

return 0;
}
