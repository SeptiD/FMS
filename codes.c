//
// Created by sd on 05.03.2018.
//
#include <stdio.h>
void print_action(int code)
{
    switch(code){
        case 0: printf("OK\n");
            break;
        case 1: printf("Establishing Connection\n");
            break;
        case 2: printf("Establishing Connection Acknowledge\n");
            break;
        case 3: printf("Request file\n");
            break;
        case 4: printf("Request file acknowledge\n");
            break;
        case 5: printf("Request tree file\n");
            break;
        case 6: printf("Request tree file acknowledge\n");
            break;
        case 7: printf("Error\n");
            break;
        case 8: printf("End connection\n");
            break;
        case 9: printf("End connection acknowledge\n");
            break;
        default:printf("I don't know this code\n");
    }
}