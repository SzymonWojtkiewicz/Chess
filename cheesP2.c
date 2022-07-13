#include <cstdio>
#include <iostream>
#include <cstdlib>
#include<stdio.h>
#include<sys/socket.h> 
#include<arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#define NSTRS       3           /* no. of strings  */
#define ADDRESS     "mysocket"

void error(const char* msg)
{
    perror(msg);
    exit(1);
}
struct moves {
    int fromLetter;
    int fromNumber;
    int toLetter;
    int toNumber;
    struct moves* previous;
    struct next* next;

};

#define PORT 7000
#define QUEUE 20
int conn;
void thread_task() {
}



/*struct  pieces {

    /*
    std::string one;
    std::string two;
    std::string three;
    std::string four;
    std::string five;
    std::string six;
    std::string seven;

}emptyWh, emptyBl, pawnWh, pawnBl, rookWh, rookBl, bishopWh, bishopBl, knightWh, knightBl, queenWh, queenBl, kingWh, kingBl;*/

int board[8][8];
int testBoard[8][8];
int nextMove[4];
std::string pieces[13][7];
int player = 2;
//char messege[4];
std::string messege = "0000";

bool castlingRookWhL = true;
bool castlingRookWhR = true;
bool castlingKingWh = true;
bool castlingRookBlL = true;
bool castlingRookBlR = true;
bool castlingingBl = true;

void draw() {
    for (int j = 0; j < 8; j++) {
        for (int k = 0; k < 7; k++) {
            for (int i = 0; i < 8; i++) {
                int z = board[i][j];
                z = z + 6;
                std::cout << pieces[z][k] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n\n\n";
    }
}



bool checkMove(int fx, int fy, int tx, int ty, int player) {

    if (fx == tx && fy == ty)
        return false;
    if (player < 1 || player > 2)
        return false;
    if (7 < fx || fx < 0 || 7 < fy || fy < 0 || 7 < tx || tx < 0 || 7 < ty || ty < 0)
        return false;
    if (player == 1) {
        if (board[fx][fy] < 1 || board[fx][fy] > 6)
            return false;
        if (board[tx][ty] < -6 || board[tx][ty] > 0)
            return false;
    }
    if (player == 2) {
        if (board[fx][fy] < -6 || board[fx][fy] > -1)
            return false;
        if (board[tx][ty] < 0 || board[tx][ty] > 6)
            return false;
    }

    //rock
    if (board[fx][fy] == -4 || board[fx][fy] == 4) {
        if (fx == tx) {
            int a;
            if (fy < ty)
                a = 1;
            else
                a = -1;
            fy = fy + a;
            while (fy != ty) {
                if (board[fx][fy] != 0)
                    return false;
                fy = fy + a;
            }
            return true;
        }
        else if (fy == ty) {
            int a;
            if (fx < tx)
                a = 1;
            else
                a = -1;
            fx = fx + a;
            while (fx != tx) {
                if (board[fx][fy] != 0)
                    return false;
                fx = fx + a;
            }
            return true;
        }
        else {
            return false;
        }
    }

    //bishop
    if (board[fx][fy] == -2 || board[fx][fy] == 2) {
        if (fx + fy == tx + ty || fx - fy == tx - ty) {
            int a, b;
            if (fx < tx)
                a = 1;
            else
                a = -1;
            if (fy < ty)
                b = 1;
            else
                b = -1;

            fx = fx + a;
            fy = fy + b;

            while (fx != tx) {
                if (board[fx][fy] != 0)
                    return false;
                fx = fx + a;
                fy = fy + b;
            }
            return true;
        }
        return false;
    }

    //queen
    if (board[fx][fy] == -5 || board[fx][fy] == 5) {
        if (fx == tx) {
            int a;
            if (fy < ty)
                a = 1;
            else
                a = -1;
            fy = fy + a;
            while (fy != ty) {
                if (board[fx][fy] != 0)
                    return false;
                fy = fy + a;
            }
            return true;
        }
        else if (fy == ty) {
            int a;
            if (fx < tx)
                a = 1;
            else
                a = -1;
            fx = fx + a;
            while (fx != tx) {
                if (board[fx][fy] != 0)
                    return false;
                fx = fx + a;
            }
            return true;
        }
        else if (fx + fy == tx + ty || fx - fy == tx - ty) {
            int a, b;
            if (fx < tx)
                a = 1;
            else
                a = -1;
            if (fy < ty)
                b = 1;
            else
                b = -1;

            fx = fx + a;
            fy = fy + b;

            while (fx != tx) {
                if (board[fx][fy] != 0)
                    return false;
                fx = fx + a;
                fy = fy + b;
            }
            return true;
        }
        return false;
    }

    //knight
    if (board[fx][fy] == -3 || board[fx][fy] == 3) {
        if ((abs(fx - tx) == 1 && abs(fy - ty) == 2) || (abs(fx - tx) == 2 && abs(fy - ty) == 1)) {
            return true;
        }
        return false;
    }

    //pawn white
    if (board[fx][fy] == 1) {
        if (fy == 1 && ty == 3) {
            if (fx != tx || board[fx][2] != 0)
                return false;
            return true;
        }

        if (fx == tx && ty - fy == 1 && board[tx][ty] == 0)
            return true;

        if ((fx - tx == -1 || fx - tx == 1) && ty - fy == 1 && board[tx][ty] < 0)
            return true;
        return false;
    }

    //pawn black
    if (board[fx][fy] == -1) {
        if (fy == 6 && ty == 4) {
            if (fx != tx || board[fx][5] != 0)
                return false;
            return true;
        }

        if (fx == tx && fy - ty == 1 && board[tx][ty] == 0)
            return true;

        if ((tx - fx == -1 || tx - fx == 1) && fy - ty == 1 && board[tx][ty] < 0)
            return true;
        return false;
    }

    //king
    if (board[fx][fy] == 6 || board[fx][fy] == -6) {
        if (abs(fx - fy) == 1 || abs(fy - ty) == 1)
            return true;
        return false;
    }

}

bool moveScan() {
    int fy, ty;
    char fx, tx;
    bool ok = true;
    std::string s = "abcdefgh";

    while (true)
    {
        std::cout << "\nColumn from:\n";
        std::cin >> fx;
        std::cout << " Row from:\n";
        std::cin >> fy;

        std::cout << "\nColumn to:\n";
        std::cin >> tx;
        std::cout << " Row to:\n";
        std::cin >> ty;

        if (!(s.find(fx) != std::string::npos) || !(s.find(tx) != std::string::npos) || fy < 1 || fy > 8 || ty < 1 || ty > 8) {
            std::cout << "wrong input try again\n";
            continue;
        }

        nextMove[0] = 0;

        for (int i = 0; s[i] != fx; i++) {
            nextMove[0]++;
        }

        nextMove[2] = 0;

        for (int i = 0; s[i] != tx; i++) {
            nextMove[2]++;
        }

        nextMove[1] = fy - 1;
        nextMove[3] = ty - 1;

        if (checkMove(nextMove[0], nextMove[1], nextMove[2], nextMove[3], player)) {

            ok = false;
            break;
        }
        else {
            std::cout << "wrong input try again\n";
            continue;
        }

    }

    return true;

}

void applayMove() {
    board[nextMove[2]][nextMove[3]] = board[nextMove[0]][nextMove[1]];
    board[nextMove[0]][nextMove[1]] = 0;

}

bool checkLegSt() {
    return true;
}

int winCheck() {
    return 0;
}

void moveToString() {
    messege = "0000";
    std::sprintf(&messege[0], "%d", nextMove[0]);
    std::sprintf(&messege[1], "%d", nextMove[1]);
    std::sprintf(&messege[2], "%d", nextMove[2]);
    std::sprintf(&messege[3], "%d", nextMove[3]);
    // messege[1] = nextMove[1];
    // messege[2] = nextMove[2];
    // messege[3] = nextMove[3];
}

void stringToMove() {
    /*
    nextMove[0] = (int)messege[0];
    nextMove[1] = (int)messege[1];
    nextMove[2] = (int)messege[2];
    nextMove[3] = (int)messege[3];
    */
    nextMove[0] = messege[0] - 48;
    nextMove[1] = messege[1] - 48;
    nextMove[2] = messege[2] - 48;
    nextMove[3] = messege[3] - 48;
}

void bugtest() {
    std::cout << "next move " << nextMove[0] << " " << nextMove[1] << " " << nextMove[2] << " " << nextMove[3] << "\n";
    std::cout << messege << "\n";
    std::cout << "messege " << messege[0] << " " << messege[1] << " " << messege[2] << " " << messege[3] << "\n";
}
int main(int argc, char* argv[])
{
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 8; i++) {
            board[i][j] = 0;
        }
    }


    if (true) {
        for (int i = 0; i < 8; i++) {
            board[i][1] = 1;
        }
        for (int i = 0; i < 8; i++) {
            board[i][6] = -1;
        }
        board[0][0] = 4;
        board[1][0] = 3;
        board[2][0] = 2;
        board[3][0] = 5;
        board[4][0] = 6;
        board[5][0] = 2;
        board[6][0] = 3;
        board[7][0] = 4;

        board[0][7] = -4;
        board[1][7] = -3;
        board[2][7] = -2;
        board[4][7] = -6;
        board[3][7] = -5;
        board[5][7] = -2;
        board[6][7] = -3;
        board[7][7] = -4;
    }
    if (true) {
        /*
        pieces[6][0] = "       ";
        pieces[6][1] = "       ";
        pieces[6][2] = "       ";
        pieces[6][3] = "       ";
        pieces[6][4] = "       ";
        pieces[6][5] = "       ";
        pieces[6][6] = "       ";
        */

        pieces[7][0] = "   #   ";
        pieces[7][1] = "  ###  ";
        pieces[7][2] = "   #   ";
        pieces[7][3] = " ##### ";
        pieces[7][4] = " ##### ";
        pieces[7][5] = "  ###  ";
        pieces[7][6] = "#######";

        pieces[10][0] = "### ###";
        pieces[10][1] = "#######";
        pieces[10][2] = " ##### ";
        pieces[10][3] = " ##### ";
        pieces[10][4] = "  ###  ";
        pieces[10][5] = "  ###  ";
        pieces[10][6] = "  ###  ";

        pieces[8][0] = "#######";
        pieces[8][1] = "#  #  #";
        pieces[8][2] = "#######";
        pieces[8][3] = "#  #  #";
        pieces[8][4] = "# ### #";
        pieces[8][5] = "#######";
        pieces[8][6] = "#######";

        pieces[9][0] = "   #   ";
        pieces[9][1] = "   #   ";
        pieces[9][2] = "#######";
        pieces[9][3] = "   #   ";
        pieces[9][4] = "   #   ";
        pieces[9][5] = "   #   ";
        pieces[9][6] = "   #   ";

        pieces[11][0] = "   #   ";
        pieces[11][1] = "  ###  ";
        pieces[11][2] = "#######";
        pieces[11][3] = "  ###  ";
        pieces[11][4] = "   #   ";
        pieces[11][5] = "   #   ";
        pieces[11][6] = "   ####";

        pieces[12][0] = "#  #  #";
        pieces[12][1] = "# ### #";
        pieces[12][2] = "#######";
        pieces[12][3] = "#######";
        pieces[12][4] = "  ###  ";
        pieces[12][5] = "  ###  ";
        pieces[12][6] = "#######";


        pieces[6][0] = "-------";
        pieces[6][1] = "-------";
        pieces[6][2] = "-------";
        pieces[6][3] = "-------";
        pieces[6][4] = "-------";
        pieces[6][5] = "-------";
        pieces[6][6] = "-------";


        pieces[5][0] = "   *   ";
        pieces[5][1] = "  ***  ";
        pieces[5][2] = "   *   ";
        pieces[5][3] = " ***** ";
        pieces[5][4] = " ***** ";
        pieces[5][5] = "  ***  ";
        pieces[5][6] = "*******";

        pieces[2][0] = "*** ***";
        pieces[2][1] = "*******";
        pieces[2][2] = " ***** ";
        pieces[2][3] = " ***** ";
        pieces[2][4] = "  ***  ";
        pieces[2][5] = "  ***  ";
        pieces[2][6] = "  ***  ";

        pieces[4][0] = "*******";
        pieces[4][1] = "*  *  *";
        pieces[4][2] = "*******";
        pieces[4][3] = "*  *  *";
        pieces[4][4] = "* *** *";
        pieces[4][5] = "*******";
        pieces[4][6] = "*******";

        pieces[3][0] = "   *   ";
        pieces[3][1] = "   *   ";
        pieces[3][2] = "*******";
        pieces[3][3] = "   *   ";
        pieces[3][4] = "   *   ";
        pieces[3][5] = "   *   ";
        pieces[3][6] = "   *   ";

        pieces[1][0] = "   *   ";
        pieces[1][1] = "  ***  ";
        pieces[1][2] = "*******";
        pieces[1][3] = "  ***  ";
        pieces[1][4] = "   *   ";
        pieces[1][5] = "   *   ";
        pieces[1][6] = "   ****";

        pieces[0][0] = "*  *  *";
        pieces[0][1] = "* *** *";
        pieces[0][2] = "*******";
        pieces[0][3] = "*******";
        pieces[0][4] = "  ***  ";
        pieces[0][5] = "  ***  ";
        pieces[0][6] = "*******";


    }

    /*
    draw();
    moveScan();
    applayMove();
    draw();
    */


    int client;
    int portNum = 1500; 
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];
    char* ip = "127.0.0.1";

    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0)
    {
        std::cout << "\nError establishing socket..." << std::endl;
        exit(1);
    }


    std::cout << "\n=> Socket client has been created..." << std::endl;

 

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);


    if (connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0)
        std::cout << "=> Connection to the server port number: " << portNum << std::endl;



    std::cout << "=> Awaiting confirmation from the server..." << std::endl; //line 40
    //recv(client, buffer, bufsize, 0);
    std::cout << "=> Connection confirmed, you are good to go...";

    std::cout << "\n\n=> Enter # to end the connection\n" << std::endl;

   



    while (true) {
        draw();
        bugtest();
        //ecv(network_socket, &messege, sizeof(messege), 0);
        std::cout << "waiting for relay\n";
        recv(client, buffer, bufsize, 0);
        std::cout << buffer[0] << buffer[1] << buffer[2] << buffer[3] << "\n";
        messege[0] = buffer[0];
        messege[1] = buffer[1];
        messege[2] = buffer[2];
        messege[3] = buffer[3];
        //recvfrom(network_socket, &messege, sizeof(messege), 0, (struct sockaddr*)&server_adress, &slen);
        //recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)
        stringToMove();
        bugtest();
        puts("Reply received\n");

        applayMove();
        bugtest();
        draw();
        do {
            moveScan();
        } while (!checkLegSt());
        applayMove();
        draw();
        //if (winCheck != 0) {
        //   break;
        //}
        moveToString();
        sleep(5);
        buffer[0] = messege[0];
        buffer[1] = messege[1];
        buffer[2] = messege[2];
        buffer[3] = messege[3];
        send(client, buffer, bufsize, 0);
        // sendto(network_socket, &messege, sizeof(messege), 0, (struct sockaddr*)&server_adress, sizeof(server_adress));
         //sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)

        puts("Data Send\n");
    }





    return 0;
}
