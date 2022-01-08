#include <bits/stdc++.h>
#include <winbgim.h>
#include <graphics.h>
#include <fstream>
#include <conio.h>

using namespace std;

struct node
{
    string info;
    node* right;
    node* left;
};

bool isRunning = true;

bool inputFull = false;
bool derivTooBig = false;
bool isOk = true;
bool irreducible = false;
bool conversionDone = false;
bool saved = false;

// buttons
int mx, my;
bool mouseClicked = false;
bool loadFromFile = false;
bool errorEmptyString = false;
bool saveInFile = false;
bool tryAgain = false;
bool deriveButton = false;
bool infoButton = false;
bool backButton = false;
bool drawButton = false;
bool exitButton = false;
int firstWindow;
int secondWindow;
int thirdWindow;

int activePage = 1;
int yPoz = 55;
int yPozD = 255;
vector<string> derivatives;

string inputString;
vector<string> priorit;
int nrDeriv = 0;

char randuri[200][200];
int nrRanduri = 0;
int inceput = 0;

char randuriD[200][200];
int nrRanduriD = 0;
int inceputD = 0;

char errors[200][200];
int nrErori = 0;

string postfixDeriv;
string infixDeriv;

node* rootF;
node* rootD;

void reset();
FILE* sav;

void initialization()
{
    initwindow( 1000, 600, "Derivare formala", 125, 25);

    // Prioritatea operatorilor

    priorit.push_back("+");
    priorit.push_back("-");
    priorit.push_back("*");
    priorit.push_back("/");
    priorit.push_back("^");
    priorit.push_back("sqrt");
    priorit.push_back("lg");
    priorit.push_back("log");
    priorit.push_back("ln");
    priorit.push_back("sin");
    priorit.push_back("cos");
    priorit.push_back("tg");
    priorit.push_back("ctg");
    priorit.push_back("arcsin");
    priorit.push_back("arccos");
    priorit.push_back("arctg");
    priorit.push_back("arcctg");

    firstWindow = getcurrentwindow();

    sav = fopen("save_derivative.txt", "w");
}
bool detectNumber(string nr)
{
    for( int i = 0; i<nr.size(); i++)
        if( nr[i]<'0' or nr[i]>'9' )
            return false;
    return true;
}
bool detectArgument(node* n)
{
    if( n!=NULL )
    {
        if ( detectNumber(n->info) == 1 or n->info == "x" )
            return true;
        detectArgument( n->left);
        detectArgument( n->right);
        return false;
    }
}
bool validCh( char c )
{
    if( c>='0' and c<='9')
        return true;
    if( c>='a' and c<='z')
        return true;
    if( c=='+' or c=='/' or c=='*' or c=='^' or c=='-' )
        return true;
    if( c==')' or c=='(' or c=='.')
        return true;
    return false;
}
bool evaluate(string fun)
{
    //nr de paranteze
    //paranteze goale
    //cazul in care se incepe direct cu operator cu aritate 2
    //cazul in care nu exista al doilea operand (operator langa operator sau operator langa NULL sau paranteza)
    //cazul in care apar alte caractere in afara de numere si x ca operatori-
    //fara spatii
    //ceva supra 0
    //mereu ceva inainte si dupa '.', cifre

    bool success = true;

    for(int i = 0; i <= nrErori; i++)
    {
        strcpy(errors[i], " ");
    }

    nrErori = 0;

    if(fun.size() == 0)
    {
        return false;
    }

    int ps, pd;
    ps = pd = 0;
    for(int i = 0; i<fun.size(); i++)
    {
        //numara parantezele
        //paranteze goale
        if( fun[i] == '(' )
        {
            ps++;
            if( fun[i+1]==')' or fun[i+1]==' ')
            {

                strcpy(errors[nrErori], "There are paranthesis with nothing in between them.");
                nrErori++;
                success = false;
            }
            //paranteza incepe cu operator2
            if( fun[i+1]=='+' or fun[i+1]=='/' or fun[i+1]=='*' or fun[i+1]=='^')
            {
                strcpy(errors[nrErori], "Paranthesis begins with a binary operator.");
                nrErori++;
                success = false;
            }
        }
        //operator2 langa operator2
        if( fun[i]=='+' or fun[i]=='/' or fun[i]=='*' or fun[i]=='^' or fun[i]=='-')
            if( fun[i+1]=='+' or fun[i+1]=='/' or fun[i+1]=='*' or fun[i+1]=='^' or fun[i+1]=='-')
            {
                strcpy(errors[nrErori], "There are binary operators next to binary operators.");
                nrErori++;
                success = false;
            }
        //operator la sfarsitul sirului sau parantezei
        if( fun[i]=='+' or fun[i]=='/' or fun[i]=='*' or fun[i]=='^' or fun[i]=='-')
            if( fun[i+1]==NULL or fun[i+1]==')')
            {
                strcpy(errors[nrErori], "The function ends with an operator or there are paranthesis that end with an operator.");
                nrErori++;
                success = false;
            }
        //alte caractere
        if( validCh(fun[i])==0)
        {
            strcpy(errors[nrErori], "The function contains other characters.");
            nrErori++;
            success = false;
        }
        if( fun[i] == ')')
            pd++;
        //spatiu in sir
        if( fun[i] == ' ')
        {
            strcpy(errors[nrErori], "There is whitespace in your input.");
            nrErori++;
            success = false;
        }
        //impartire la 0
        if( fun[i] == '/' )
            if( fun[i+1] == '0' )
            {
                strcpy(errors[nrErori], "The function contains dividing by 0.");
                nrErori++;
                success = false;
            }
        //ceva inainte si dupa '.'
        if( fun[i] == '.' )
            if( fun[i-1]<'0' or fun[i-1]>'9' or fun[i+1]<'0' or fun[i+1]>'9' )
            {
                strcpy(errors[nrErori], "The function contains an incomplete or incorrect number.");
                nrErori++;
                success = false;
            }
    }
    //incepe cu operator2
    if( fun[0]=='+' or fun[0]=='/' or fun[0]=='*' or fun[0]=='^')
    {
        strcpy(errors[nrErori], "Your input begins with a binary operator.");
        nrErori++;
        success = false;
    }
    if ( ps != pd )
    {
        strcpy(errors[nrErori], "The number of open paranthesis does not match the number of closed paranthesis.");
        nrErori++;
        success = false;
    }
    return success;
}
bool evaluate1String( string x)
{
    for(int j = 5; j <= 16; j++)
        if(x == priorit[j])
            return true;
    return false;
}
bool evaluateStrings(string fun)
{

    bool success = true;
    string x="";
    if(fun == "x")
        return success;
    if(fun.size() > 1 && fun[0] == 'x' && (fun[1] < 'a' || fun[1] > 'z'))
        return success;
    for(int i = 0; i <= fun.size(); i++)
    {
        if(fun[i]>='a' and fun[i]<='z')
            x+=fun[i];
        else
        {
            if(x!= "x"  and x!="" and x!="e")
            {
                if(evaluate1String(x) == 0)
                {
                    strcpy(errors[nrErori], "There is a function written incorrectly.");
                    nrErori++;
                    success = false;
                }
                else
                {
                    if(fun[i] != '(')
                    {
                        strcpy(errors[nrErori], "There are functions with no arguments.");
                        nrErori++;
                        success = false;
                    }
                }

            }
            x="";
        }
    }
    return success;
}
bool detectPriorit(string pr)
{
    for( int i = 0; i<=17; i++)
        if( priorit[i] == pr)
            return true;
    return false;
}
bool operand(string inputString)
{
    if(inputString[0] == '(' || inputString[0] == ')')
        return false;
    for(int i = 0; i < 17; i++)
        if(inputString == priorit[i])
            return false;
    return true;
}
int priority(string inputString)
{
    for(int i = 0; i < 17; i++)
        if(inputString == priorit[i])
            return i;
    return 17;
}
queue<string> convInfix2Postfix(string seq)
{
    stack<string> st;
    queue<string> postfix;

    int i = 0;

    while(i < seq.size())
    {
        string x = "";

        while(seq[i] >= 'a' && seq[i] <= 'z')
        {
            x += seq[i];
            i++;
        }

        while(seq[i] >= '0' && seq[i] <= '9' or seq[i]=='.')
        {
            x += seq[i];
            i++;
        }

        if(x == "")
        {
            x += seq[i];
            i++;
        }
        if(operand(x))
        {
            postfix.push(x);
        }
        else
        {
            if(x == ")")
            {
                while(st.top() != "(")
                {
                    postfix.push(st.top());
                    st.pop();
                }
                st.pop();
            }
            else
            {
                while(!st.empty() && st.top() != "(" && (priority(st.top()) >= priority(x)))
                {
                    postfix.push(st.top());
                    st.pop();
                }
                st.push(x);
            }
        }
    }

    while(!st.empty())
    {
        postfix.push(st.top());
        st.pop();
    }

    return postfix;
}
void minusUnar(string& fun)
{
    if( fun[0] == '-')
        fun = "0" + fun;
    for( int i = 0; i < fun.size(); i++)
        if( fun[i] == '(' and fun[i+1] == '-' )
            fun.insert(i+1, 1, '0');
}
void partiallyreset()
{
    conversionDone = false;
    nrDeriv = 0;

    derivatives.clear();
    for(int i = 0; i <= nrErori; i++)
    {
        strcpy(errors[i], " ");
    }
    nrErori = 0;

    for(int i = 0; i <= nrRanduri; i++)
    {
        strcpy(randuri[i], " ");
    }
    nrRanduri = 0;
    inceput = 0;

    inputFull = false;
}
void getInput()
{
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        mouseClicked = true;
        getmouseclick(WM_LBUTTONDOWN, mx, my);
    }
    else
    {
        mouseClicked = false;
    }

    if(conversionDone == true and mouseClicked == true && mx >= 880 and mx <= 940 and my >= 420 and my <= 440)
        drawButton = true;
    else
        drawButton = false;
    if(mx>= 620 and mx<= 655 and my>=500 and my<=530)
        exitButton=true;
    else
        exitButton = false;
    if(infoButton == false)
    {

        if(mouseClicked == true and mx>= 50 and mx<= 200 and my>=500 and my<=530)
            deriveButton = true;
        else
            deriveButton = false;

        if(mouseClicked == true and mx>= 280 and mx<= 520 and my>=45 and my<=75)
            loadFromFile = true;
        else
            loadFromFile = false;

        if(mouseClicked == true and mx>= 220 and mx<= 455 and my>=500 and my<=530)
            saveInFile = true;
        else
            saveInFile = false;

        if(mouseClicked == true and mx>= 475 and mx<= 545 and my>=500 and my<=530)
            tryAgain = true;
        else
            tryAgain = false;

        if(mouseClicked == true and mx>= 565 and mx<= 600 and my>=500 and my<=530)
            infoButton = true;
    }
    else
    {
        if(mouseClicked == true and mx>= 900 and mx<= 940 and my>=530 and my<=560)
            backButton = true;
        else
            backButton = false;
    }



    if(kbhit())
    {
        char key = getch();
        if(key != 0 && inputFull == false)
        {
            if(conversionDone == true)
            {
                partiallyreset();
                cleardevice();
            }
            if(key == '\b' && inputString.size() != 0)
            {
                inputString.erase(inputString.end()-1);
            }
            else
            {
                if(key != '\b')
                    inputString += key;
            }
            if(inputString.size() == 999)
                inputFull = true;
        }
        if(evaluate(inputString) == false || evaluateStrings(inputString) == false)
            isOk = false;
        else
            isOk = true;
    }


}
int arity(string q)
{
    for( int i=0; i<5; i++)
        if( priorit[i] == q)
            return 2;
    return 1;
}
node* generateTree(stack<string> &St)
{
    node* root = new node;
    if(St.empty())
        return NULL;
    if(operand(St.top()))
    {
        root->info = St.top();
        St.pop();
        root->right = NULL;
        root->left = NULL;
    }
    else
    {
        root->info = St.top();
        St.pop();
        if(arity(root->info) == 2)
        {
            root->right = generateTree(St);
            root->left = generateTree(St);
        }
        else
        {
            root->right = generateTree(St);
            root->left = NULL;
        }
    }
    return root;
}
void generateStack()
{
    stack<string> St;
    string inputString2 = inputString;
    minusUnar(inputString2);
    queue<string> cd = convInfix2Postfix(inputString2);
    while(!cd.empty())
    {
        St.push( cd.front());
        cd.pop();
    }

    rootF = generateTree(St);

}
int isNull(node* a)
{
    return (a==NULL);
}
bool detectFunction(node* fun)
{
    if(fun == NULL)
        return false;
    if(fun->info == "x")
        return true;
    return (detectFunction(fun->left) || detectFunction(fun->right));
}
node* derive(node* root)
{
    node* n = new node;
    if(operand(root->info))
    {
        n->right = NULL;
        n->left = NULL;
        if(root->info == "x")
            n->info = "1";
        else
            n->info = "0";
        return n;
    }

    if(root->info == "+")
    {
        n->info = "+";
        n->left = derive(root->left);
        n->right = derive(root->right);
        return n;
    }

    if(root->info == "-")
    {
        n->info = "-";
        n->left = derive(root->left);
        n->right = derive(root->right);
        return n;
    }

    if(root->info == "*")
    {
        node* p = new node;
        node* d = new node;
        p->info = "*";
        p->left = derive(root->left);
        p->right = root->right;

        d->info = "*";
        d->left = root->left;
        d->right = derive(root->right);

        n->info = "+";
        n->left = p;
        n->right = d;

        return n;
    }
    if(root->info == "/")
    {
        node* p = new node;
        node* d = new node;
        p->info = "*";
        p->left = derive(root->left);
        p->right = root->right;

        d->info = "*";
        d->left = root->left;
        d->right = derive(root->right);


        node* t = new node;
        t->info = "-";
        t->left = p;
        t->right = d;

        node* j = new node;
        j->info = "^";
        j->left = root->right;
        j->right = new node;
        j->right->left=NULL;
        j->right->right=NULL;
        j->right->info = "2";

        n->info = "/";
        n->left = t;
        n->right = j;

        return n;
    }

    if(root->info == "^")
    {
        if(detectFunction(root->left) && detectFunction(root->right))
        {
            n->info = "ln";
            n->right = root->left;
            n->left = NULL;

            node* n1 = new node;
            n1->info = "*";
            n1->left = root->right;
            n1->right = n;

            node* r = new node;
            r->info = "^";
            r->left = new node;
            r->left->info = "e";
            r->left->left = NULL;
            r->left->right = NULL;
            r->right = n1;

            return derive(r);
        }
        else

        if(detectFunction(root->left))
        {
            node* n1 = new node;
            n1->info = "-";
            n1->left = new node;
            n1->left = root->right;
            n1->right = new node;
            n1->right->left = NULL;
            n1->right->right = NULL;
            n1->right->info = "1";

            node* p = new node;
            p->info = "^";
            p->left = root->left;
            p->right = n1;

            n->info = "*";
            n->left = new node;
            n->left->left = NULL;
            n->left->right = NULL;
            n->left = root->right;
            n->right = p;

            node* r = new node;
            r->info = "*";
            r->left = n;
            r->right = derive(root->left);

            return r;
        }
        else

        if(detectFunction(root->right))
        {
            node* n1=new node;
            n1->info ="ln";
            n1->left=NULL;
            n1->right=new node;
            n1->right->right=NULL;
            n1->right->left=NULL;
            n1->right = root->left;

            n->info="*";
            n->left=root;
            n->right=n1;

            node* r = new node;
            r->info = "*";
            r->left = n;
            r->right = derive(root->right);

            return r;
        }
        else
        {
            n->info = "0";
            n->left = NULL;
            n->right = NULL;

            return n;
        }
    }
    if(root->info == "sqrt")
    {
        node* n3 = new node;
        n3->info = "sqrt";
        n3->right = new node;
        n3->right = root->right;
        n3->left = NULL;

        node* n1 = new node;
        n1->info = "*";
        n1->left = new node;
        n1->left->info = "2";
        n1->left->left= NULL;
        n1->left->right =NULL;
        n1->right = n3;

        n->info = "/";
        n->left = new node;
        n->left->info = "1";
        n->left->left = NULL;
        n->left->right = NULL;
        n->right = n1;

        node* r = new node;
        r->info = "*";
        r->right = derive(root->right);
        r->left = n;

        return r;

    }
    if(root->info == "log" || root->info == "ln")
    {
        n->info = "/";
        n->left = new node;
        n->left->left = NULL;
        n->left->right = NULL;
        n->left->info = "1";
        n->right = root->right;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;
    }
    if(root->info == "lg")
    {
        node* n1 = new node;
        n1->info = "ln";
        n1->right = new node;
        n1->right = root->right;
        n1->left = NULL;

        node* n2 = new node;
        n2->info = "ln";
        n2->right = new node;
        n2->right->info = "10";
        n2->right->right = NULL;
        n2->right->left = NULL;
        n2->left = NULL;

        node* r = new node;
        r->info = "/";
        r->left = derive(n1);
        r->right = n2;

        return r;
    }
    if(root->info == "sin")
    {
        n->info = "cos";
        n->right = root->right;
        n->left = NULL;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;
    }
    if(root->info == "cos")
    {
        node* p = new node;
        p->info = "sin";
        p->right = root->right;
        p->left =NULL;

        n->info = "-";
        n->left = new node;
        n->left->info = "0";
        n->left->right = NULL;
        n->left->left = NULL;
        n->right = p;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;
    }
    if(root->info == "tg")
    {
        node* q = new node;
        q = root->right;

        node* p= new node;
        p->info = "^";
        p->left = new node;
        p->left->info= "cos";
        p->left->right = q;
        p->left->left = NULL;
        p->right = new node;
        p->right->info = "2";
        p->right->right=NULL;
        p->right->left=NULL;

        n->info = "/";
        n->left = new node;
        n->left->info = "1";
        n->right = p;
        n->left->right=NULL;
        n->left->left=NULL;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;

    }
    if(root->info == "ctg")
    {
        node* r= new node;
        r = root->right;

        node* q= new node;
        q->info = "^";
        q->left = new node;
        q->left->info= "sin";
        q->left->right = r;
        q->left->left = NULL;
        q->right = new node;
        q->right->info = "2";
        q->right->left=NULL;
        q->right->right=NULL;

        node* p= new node;
        p->info = "/";
        p->left = new node;
        p->left->info = "1";
        p->left->right =NULL;
        p->left->left=NULL;
        p->right = q;

        n->info = "-";
        n->left = new node;
        n->left->info="0";
        n->left->left=NULL;
        n->left->right=NULL;
        n->right = p;

        node* k = new node;
        k->info = "*";
        k->left = n;
        k->right = derive(root->right);

        return k;
    }
    if(root->info == "arcsin")
    {
        node* n3= new node;
        n3->info= "^";
        n3->left = new node;
        n3->left = root->right;
        n3->right = new node;
        n3->right->right=NULL;
        n3->right->left=NULL;
        n3->right->info= "2";

        node* n2= new node;
        n2->info = "-";
        n2->left = new node;
        n2->left->right=NULL;
        n2->left->left=NULL;
        n2->left->info = "1";
        n2->right = n3;

        node* n1= new node;
        n1->info= "sqrt";
        n1->left=  NULL;
        n1->right= n2;

        n->info = "/";
        n->left = new node;
        n->left->info = "1";
        n->left->right=NULL;
        n->left->left=NULL;
        n->right = n1;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;
    }
    if(root->info == "arccos")
    {
        node* n3= new node;
        n3->info= "^";
        n3->left = new node;
        n3->left = root->right;
        n3->right = new node;
        n3->right->info= "2";
        n3->right->right=NULL;
        n3->right->left=NULL;

        node* n2= new node;
        n2->info = "-";
        n2->left = new node;
        n2->left->info = "1";
        n2->left->right=NULL;
        n2->left->left=NULL;
        n2->right = n3;

        node* n1= new node;
        n1->info= "sqrt";
        n1->left=  NULL;
        n1->right= n2;

        n->info = "/";
        n->left = new node;
        n->left->right=NULL;
        n->left->left=NULL;
        n->left->info = "1";
        n->right = n1;

        node* p = new node;
        p->info = "-";
        p->left = new node;
        p->left->info="0";
        p->left->right=NULL;
        p->left->left=NULL;
        p->right = n;

        node* r = new node;
        r->info = "*";
        r->left = p;
        r->right = derive(root->right);

        return r;
    }

    if(root->info == "arctg")
    {
        node* n3= new node;
        n3->info="^";
        n3->left=new node;
        n3->left = root->right;
        n3->right=new node;
        n3->right->info="2";
        n3->right->left=NULL;
        n3->right->right=NULL;

        node* n2=new node;
        n2->info="+";
        n2->left=n3;
        n2->right=new node;
        n2->right->info="1";
        n2->right->right=NULL;
        n2->right->left=NULL;

        n->info="/";
        n->right=n2;
        n->left=new node;
        n->left->info="1";
        n->left->right=NULL;
        n->left->left=NULL;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;
    }
    if(root->info == "arcctg")
    {
        node* n3= new node;
        n3->info="^";
        n3->left=new node;
        n3->left = root->right;
        n3->right=new node;
        n3->right->info="2";
        n3->right->left=NULL;
        n3->right->right=NULL;

        node* n2=new node;
        n2->info="+";
        n2->left=n3;
        n2->right=new node;
        n2->right->info="1";
        n2->right->right=NULL;
        n2->right->left=NULL;

        node* n1 = new node;
        n1->info="/";
        n1->right=n2;
        n1->left=new node;
        n1->left->info="1";
        n1->left->right=NULL;
        n1->left->left=NULL;

        n->info = "-";
        n->right = n1;
        n->left = new node;
        n->left->info="0";
        n->left->left=NULL;
        n->left->right=NULL;

        node* r = new node;
        r->info = "*";
        r->left = n;
        r->right = derive(root->right);

        return r;
    }
}
bool compareTrees(node* a, node* b)
{

    if(a == NULL && b == NULL)

        return true;

    if((a == NULL && b != NULL) || (a != NULL && b == NULL))

        return false;

    if(a->info != b->info)

        return false;

    return compareTrees(a->left, b->left) && compareTrees(a->right, b->right);

}
void postorder(node* a)
{
    if (!isNull(a))
    {
        postorder(a->left);
        postorder(a->right);
        postfixDeriv+=a->info;
        postfixDeriv+=" ";
    }
}
void parcurgereInPreordine(node* a, int nivel)
{
    if (!isNull(a))
    {
        for( int i =1; i<=nivel; i++)
            cout<<"   ";
        cout<<a->info<<"\n";
        parcurgereInPreordine(a->left, nivel+1);
        parcurgereInPreordine(a->right, nivel+1);
    }
}
string convPostfix2Infix(string postFix)
{
    stack<string> St;
    int i=0;

    while( i<postFix.size() )
    {
        string c="";
        while ( postFix[i] != ' ')
        {
            c+=postFix[i];
            i++;
        }
        if( operand(c) )
            St.push(c);
        else
        {
            string right="";
            string left="";
            string val="";
            right=St.top();
            St.pop();
            if( arity(c) == 2)
            {
                left=St.top();
                St.pop();
                val+="("+left+c+right+")";
            }
            else
                val+="("+c+right+")";
            //cout<<val<<endl;
            St.push(val);
        }
        i++;
    }
    return St.top();
}
string Number2String( double nr )
{
    string z;
    z=to_string(nr);
    for(int i=z.size()-1; i>=0; i--)
        if(z[i]=='0')
            z.erase(z.end()-1);
        else
        {
            if( z[z.size()-1]=='.' )
                z.erase(z.end()-1);
            return z;
        }
}
double String2Number( string number)
{
    double nr = 0;
    int c = 0;
    int dot=0;
    int p=0;
    for(int i=0; i<number.size(); i++)
    {
        if(dot==1)
            p++;
        if(number[i]=='.')
            dot=1;
    }
    for(int i = 0; i < number.size(); i++)

        if(number[i]!='.')
        {
            c = number[i] - '0';
            nr *= 10;
            nr += c;
        }
    if(dot==1)
        return nr/pow(10,p);
    else
        return nr;
}
void simplify(node*& root)
{
    if(root != NULL)
    {
        simplify(root->left);
        simplify(root->right);
        if(root->info == "*")
        {
            if(root->left->info == "0" || root->right->info == "0")
            {
                root->info = "0";
                root->left = NULL;
                root->right = NULL;
                irreducible = false;
                return;
            }
            if(root->left->info == "1")
            {
                root = root->right;
                irreducible = false;
                return;
            }
            if(root->right->info == "1")
            {
                root = root->left;
                irreducible = false;
                return;
            }

        }

        if(root->info == "+")
        {
            if( root->right->info == "0")
            {
                root = root->left;
                irreducible =  false;
                return;

            }
            if(root->left->info == "0")
            {
                root = root->right;
                irreducible = false;
                return;
            }

        }
        if(root->info == "/")
        {
            if(root->left->info == "0")
            {
                root->info = "0";
                root->right = NULL;
                root->left = NULL;
                irreducible = false;
                return;
            }
        }
        if(root->info == "-")
        {
            if( root->right->info == "0")
            {
                root = root->left;
                irreducible = false;
                return;
            }
        }
        if(root->info == "^")
        {
            if( root->left->info == "0" )
            {
                root->info = "0";
                root->left =  NULL;
                root->right = NULL;
                irreducible = false;
                return;
            }
            if( root->right->info == "0" )
            {
                root->info = "1";
                root->left = NULL;
                root->right = NULL;
                irreducible = false;
                return;
            }
            if( root->right->info == "1")
            {
                root = root->left;
                irreducible = false;
                return;
            }
            if( root->left->info == "1")
            {
                root->info = "1";
                root->right = NULL;
                root->left = NULL;
                irreducible = false;
                return;
            }
        }

    }
}
void compute(node*& root)
{
    if(root != NULL && operand(root->info) != true)
    {
        compute(root->left);
        compute(root->right);
        if(root->info == "+")
        {
            if((operand(root->right->info) && root->right->info != "x" && root->right->info != "e") && (operand(root->left->info) && root->left->info != "x" && root->left->info != "e"))
            {
                double x = String2Number(root->left->info);
                double y = String2Number(root->right->info);
                cout << x << " " << y;
                //cout << x + y << '\n';
                root->info = Number2String(x+y);
                root->left = NULL;
                root->right = NULL;
                irreducible = false;
                return;
            }
        }
        if(root->info == "-" && root->left->info != "0")
        {
            if((operand(root->right->info) && root->right->info != "x" && root->right->info != "e") && (operand(root->left->info) && root->left->info != "x" && root->left->info != "e"))
            {
                double x = String2Number(root->left->info);
                double y = String2Number(root->right->info);
                root->info = Number2String(x-y);
                root->left = NULL;
                root->right = NULL;
                irreducible = false;
                return;
            }
        }
        if(root->info == "*")
        {
            if((operand(root->right->info) && root->right->info != "x" && root->right->info != "e") && (operand(root->left->info) && root->left->info != "x" && root->left->info != "e"))
            {
                double x = String2Number(root->left->info);
                double y = String2Number(root->right->info);
                root->info = Number2String(x*y);
                root->left = NULL;
                root->right = NULL;
                irreducible = false;
                return;
            }
        }
        if( root->info == "sqrt" )
        {
            if( root->right->info == "1")
            {
                root->info = "1";
                root->right = NULL;
                root->left = NULL;
                irreducible = false;
                return;
            }
            if( root->right->info == "0")
            {
                root->info = "0";
                root->right = NULL;
                root->left = NULL;
                irreducible = false;
                return;
            }
        }
        if(root->info == "log" or root->info == "ln" )
        {
            if( root->right->info == "1")
            {
                root->info = "0";
                root->right = NULL;
                root->left = NULL;
                irreducible = false;
                return;
            }
            if( root->right->info == "e")
            {
                root->info = "1";
                root->right = NULL;
                root->left = NULL;
                irreducible = false;
                return;
            }
        }
    }
}
void math(node*& root)
{
    if(root != NULL && operand(root->info) != true)
    {
        math(root->left);
        math(root->right);
        if(root->info == "*")
        {
            if(root->right->info == "*" && isdigit(root->left->info[0]))
            {
                // adancime 3*sin(x) ^ 7
                node* p = root->right->right;
                node* c = root->right->left;
                if(isdigit(root->right->right->info[0]))
                {
                    double x = String2Number(root->left->info);
                    double y = String2Number(root->right->right->info);
                    root->left->info = Number2String(x*y);
                    root->right = root->right->left;
                }
                if(isdigit(root->right->left->info[0]))
                {
                    int x = String2Number(root->left->info);
                    int y = String2Number(root->right->left->info);
                    root->left->info = Number2String(x*y);
                    root->right = root->right->right;
                }
            }
            if(root->left->info == "*" && isdigit(root->right->info[0]))
            {
                //parcurgereInPreordine(root, 0);
                if(isdigit(root->left->right->info[0]))
                {
                    double x = String2Number(root->right->info);
                    double y = String2Number(root->left->right->info);
                    root->left->info = Number2String(x*y);
                    root->right = root->left->left;
                }
                if(isdigit(root->left->left->info[0]))
                {

                    double x = String2Number(root->right->info);
                    double y = String2Number(root->left->left->info);
                    root->left->info = Number2String(x*y);
                    root->right = root->left->right;

                }
            }
        }
        if(root->info == "+")
        {
            if(root->right->info == "*" && root->left->info == "*")
            {
                if((compareTrees(root->right->right, root->left->right) && isdigit(root->right->left->info[0])) && isdigit(root->left->left->info[0]))
                {
                    double x = String2Number(root->right->left->info);
                    double y = String2Number(root->left->left->info);
                    root->left->info = Number2String(x+y);
                    root->right = root->right->right;
                    root->info = "*";
                }
            }
        }
        if(root->info == "*")
        {
            if(root->right->info == "^" && root->left->info == "^")
            {
                if((compareTrees(root->right->left, root->left->left) && isdigit(root->right->right->info[0])) && isdigit(root->left->right->info[0]))
                {
                    double x = String2Number(root->right->right->info);
                    double y = String2Number(root->left->right->info);
                    root->right->info = Number2String(x*y);
                    root->right->right = NULL;
                    root->right->left = NULL;
                    root->left = root->left->left;
                    root->info = "^";
                }
            }
        }
    }
}
void load()
{
    FILE* cit;
    cit = fopen("load_function.txt", "r");
    char rc[1001];
    fscanf(cit, "%s", &rc);
    inputString += rc;
    fclose(cit);
}
void save()
{
    fprintf(sav, "%s%s", "Introduced/loaded function : ", inputString.c_str());

    if(derivatives.size() == 1)
    {
        fprintf(sav, "%s%s", "\nThe first derivative of the function is : ", derivatives[0].c_str());
    }

    if(derivatives.size() == 2)
    {
        fprintf(sav, "%s%s", "\nThe second order derivative of the function is : ", derivatives[1].c_str());
    }

    if(derivatives.size() == 3)
    {
        fprintf(sav, "%s%s", "\nThe third order derivative of the function is : ", derivatives[2].c_str());
    }
    fprintf(sav, "%s", "\n\n\n");
}
void reset()
{
    inputString = "";
    conversionDone = false;
    nrDeriv = 0;

    derivatives.clear();
    for(int i = 0; i <= nrErori; i++)
    {
        strcpy(errors[i], " ");
    }
    nrErori = 0;

    for(int i = 0; i <= nrRanduri; i++)
    {
        strcpy(randuri[i], " ");
    }
    nrRanduri = 0;
    inceput = 0;

    inputFull = false;
}
int nrNiveluri(node *a)
{
    if(a==NULL)return 0;
    else
    {
        int n1=nrNiveluri(a->left);
        int n2=nrNiveluri(a->right);
        return 1+max(n1,n2);
    }
}
int nrColoane(node *a)
{
    if(a==NULL)return 0;
    else
    {
        int n1=nrColoane(a->left);
        int n2=nrColoane(a->right);
        return 1+n1+n2;
    }
}
void deseneazaNod(string elem, int xc, int yc, int latime, int inaltime)
{
    if(elem != "0")
    {
    char arr[500];
    strcpy(arr, elem.c_str());
    setcolor(WHITE); setfillstyle(SOLID_FILL,LIGHTCYAN);
    fillellipse(xc,yc, textwidth(arr)+2, textheight("M")/2+6);
    settextstyle(SANS_SERIF_FONT,HORIZ_DIR,1);
    setbkcolor(LIGHTCYAN);
    setcolor(BLUE);
    settextjustify(1,1);
    outtextxy(xc,yc+4,arr);
    }
}
void drawTree(node *a, int niv, int stanga, int latime, int inaltime)
{
    if(a!=NULL)
    {
        int n1=nrColoane(a->left);
        int xc=stanga+latime*n1+latime/2;
        int yc=niv*inaltime-inaltime/2;

        if (a->left!=NULL && a->left->info != "0")
        {
            int xcs=stanga+latime*nrColoane(a->left->left)+latime/2;
            setcolor(WHITE);
            line(xc,yc,xcs,yc+inaltime);
        }
        if (a->right!=NULL && a->right->info != "0")
        {
            int xcd=stanga+latime*(n1+1)+latime*nrColoane(a->right->left)+latime/2;
            setcolor(WHITE);
            line(xc,yc,xcd,yc+inaltime);
        }
        deseneazaNod(a->info,xc,yc,latime, inaltime);
        drawTree(a->left,niv+1,stanga, latime, inaltime);
        drawTree(a->right,niv+1,stanga+latime*(n1+1), latime, inaltime);
    }

}
void handleEvents()
{
    if(activePage == 1)
        activePage = 0;
    else
        activePage = 1;
    if(loadFromFile == true)
    {
        load();
        if(inputString=="")
        {
            initwindow(150, 50, "Confirm", 500, 100, false, false);
            secondWindow = getcurrentwindow();
            setcurrentwindow(secondWindow);
            outtextxy(15, 20, "No function found!");
            delay(1500);
            closegraph(secondWindow);
        }
    }

    if(saveInFile == true and  conversionDone == true )
    {
        initwindow(100, 50, "Confirm", 400, 500, false, false);
        secondWindow = getcurrentwindow();
        setcurrentwindow(secondWindow);
        outtextxy(45, 20, "Saved!");delay(1500);
        closegraph(secondWindow);

        save();
    }

    if(tryAgain == true)
    {
        reset();
    }

    //cout << infoButton << '\n';

    if(backButton == true)
    {
        infoButton = false;
        backButton = false;
    }

    if(inputString.size() == 0)
        errorEmptyString = true;
    else
        errorEmptyString = false;

    if(drawButton == true)
    {
        int closed=0;
        initwindow(1000, 600, "Arbore", 125, 25, false, true);
        thirdWindow = getcurrentwindow();
        setcurrentwindow(thirdWindow);
        rectangle(950,570,990,590);
        outtextxy(956, 573, "EXIT");
        drawTree(rootD, 1, 9, 900/nrColoane(rootD), 550/nrNiveluri(rootD));
        while(closed!=1)
        {
            if(ismouseclick(WM_LBUTTONDOWN))
            {
                mouseClicked = true;
                getmouseclick(WM_LBUTTONDOWN, mx, my);
            }
            else
                mouseClicked = false;
            if( mouseClicked==1 and mx>950 and mx<990 and my>570 and my<590)
            {
                closegraph(thirdWindow);
                closed=1;
            }
        }
     }
     if(exitButton==true)
     {
        isRunning = false;
     }
    if(deriveButton == true && isOk == true)
    {
        saved = false;
        if(inputString.size() != 0)
        {
            if(nrDeriv < 3)
                nrDeriv++;
            generateStack();
            while(!irreducible)
            {
                irreducible = true;
                simplify(rootF);
                compute(rootF);
            }
            math(rootF);
            int c = nrDeriv;
            while(c && c < 4)
            {
                rootD = derive(rootF);
                rootF = rootD;
                if(irreducible == true)
                    irreducible = false;

                while(!irreducible)
                {
                    irreducible = true;
                    simplify(rootD);
                    compute(rootD);
                }
                math(rootF);
                c--;
            }

            postorder(rootD);
            //cout << postfixDeriv << '\n';
            infixDeriv = convPostfix2Infix(postfixDeriv);
            //cout << infixDeriv << '\n';

            conversionDone = true;
        }
    }
}
void generateOutput()
{
    setcurrentwindow(firstWindow);

    setvisualpage(activePage);
    setactivepage(!activePage);
    setcolor(BLACK);
    cleardevice();
    setcolor(WHITE);


    // prima caseta
    rectangle(50, 100, 950, 250);
    outtextxy(50, 50, "Type the function to be derived or ");
    outtextxy(10, 105, "f(x) = ");

    // a doua caseta
    rectangle(50, 300, 950, 450);

    // load
    rectangle(280, 45, 520, 75);
    outtextxy(285, 50, "Load from file \"load_function.txt\" ");

    string u ="";
    string y = "";
    switch(nrDeriv)
    {
    case 1:
        u = "The derivative of the function is : ";
        y = "f'(x) =";
        break;
    case 2:
        u = "The second order derivative of the function is : ";
        y = "f''(x) =";
        break;
    case 3:
        u = "The third order derivative of the function is : ";
        y = "f'''(x) =";
        break;
    }

    char ot[5000];
    strcpy(ot, u.c_str());
    outtextxy(50, 280, ot);
    strcpy(ot, y.c_str());
    outtextxy(5, 305, ot);

    if(infoButton == false)
    {
        if(nrDeriv == 0)
            outtextxy(55, 505, "Derive");
        else
            outtextxy(55, 505, "Next order derivative");

        // derive
        rectangle(50, 500, 200, 530);

        // save
        rectangle(220, 500, 455, 530);
        outtextxy(225, 505, "Save in file \"save_derivative.txt\" ");

        // try again
        rectangle(475, 500, 545, 530);
        outtextxy(480, 505, "Try Again");

        // info
        rectangle(565, 500, 600, 530);
        outtextxy(570, 505, "Info");

        //exit
        rectangle(620, 500, 655, 530);
        outtextxy(625,505,"Exit");
    }
    else
    {
        outtextxy(50, 505, "The operators supported by the calculator are : +, -, *, /, ^.");
        outtextxy(50, 525, "The functions supported by the calculator are : sin, cos, tg, ctg, sqrt, ln, log, lg, arcsin, arccos, arctg, arcctg.");
        outtextxy(50, 545, "Please use paranthesis to specify the arguments of the functions.");

        rectangle(900, 530, 940, 560);
        outtextxy(905, 535, "Back");
    }

    char temp[2000];
    strcpy(temp, inputString.c_str());

    if(strlen(temp) >= inceput && strlen(temp) <= inceput + 111)
        strcpy(randuri[nrRanduri], temp + inceput);
    else
    {
        nrRanduri++;
        inceput = inceput + 111;
    }

    yPoz = 105;
    for(int i = 0; i <= nrRanduri; i++)
    {
        outtextxy(55, yPoz, randuri[i]);
        yPoz += 15;
    }

    if(!isOk)
    {
        int pzy = 305;
        outtextxy(55, 305, "Invalid input: ");
        for(int i = 0; i < nrErori; i++)
        {
            outtextxy(55, pzy + ((i+1) * 15), errors[i]);
        }
    }

    if(errorEmptyString)
        outtextxy(55, 305, "Type the function using the keyboard or press the upload from file button if you want to upload your function from file.");
    else
    {
        if(conversionDone)
        {
            char temp[100000];
            if(infixDeriv.size() > 999)
            {
                outtextxy(55, 305, "The derivative is too big to be written on the screen and it was saved in save_derivative.txt.");
                if(saved == false)
                {
                    if(derivatives.size() == 0)
                        derivatives.push_back(infixDeriv);
                    else
                    {
                        if(derivatives[derivatives.size()-1] != infixDeriv)
                            derivatives.push_back(infixDeriv);
                    }
                    save();
                    saved = true;
                }
            }
            else
            {
                strcpy(temp, infixDeriv.c_str());
                for(int i = 0; i < strlen(temp) - 1; i++)
                    if((temp[i] == '0' && temp[i+1] == '-') && (temp[i-1] < '0' or temp[i-1] > '9'))
                        strcpy(temp+i, temp+i+1);
                if(temp[0] == '(' and temp[ strlen(temp) -1 ] == ')')
                {
                    strcpy(temp, temp+1);
                    temp[strlen(temp) - 1] = NULL;
                }

                string nD = "";
                for(int i = 0; i < strlen(temp); i++)
                    nD += temp[i];

                if(derivatives.size() == 0)
                    derivatives.push_back(nD);
                else
                {
                    if(derivatives[derivatives.size()-1] != nD)
                        derivatives.push_back(nD);
                }

                inceputD = 0;
                nrRanduriD = 0;

                while(inceputD < strlen(temp))
                {
                    if(strlen(temp) - inceputD > 160)
                    {
                        strncpy(randuriD[nrRanduriD], temp + inceputD, 160);
                        inceputD += 160;
                        nrRanduriD++;
                    }
                    else
                    {
                        strcpy(randuriD[nrRanduriD], temp + inceputD);
                        break;
                    }
                }

                yPozD = 305;
                for(int i = 0; i <= nrRanduriD; i++)
                {
                    outtextxy(55, yPozD, randuriD[i]);
                    yPozD += 17;
                }
            }
            rectangle(880, 420, 940, 440);
            outtextxy(885, 422, "Draw");
        }
    }
}
void quit()
{
    delete rootF;
    delete rootD;
}
int main()
{
    initialization();
    while(isRunning)
    {
        getInput();
        handleEvents();
        generateOutput();
    }
    quit();
    return 0;
}
