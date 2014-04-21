#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

struct Student
{
    string name, uid, course[6];
    int  CW[6], EX[6], cCW[6], cEX[6];
    double gpa[6];
};

void error (ifstream &fin)
{
    if (fin.fail())
    {
        cout << "Error opening file" << endl;

        exit (1);
    }
}

double readGrade (ifstream &fgrade, double score) //ok
//determine the gpa of student in that course.
{
    int LS[7], HS[7], i=0;
    double grade[7];
    char dummy;
    fgrade.clear();
    fgrade.seekg(0, ios::beg);
    while (fgrade.good())
    {
        fgrade >> LS[i] >> dummy >> HS[i] >> dummy >> grade[i];
        if (score>LS[i] && score<HS[i])
            return grade[i];
        else
            i++;
    }
    return 99.0;
}

double readCourse (ifstream &fcourse, ifstream &fgrade, Student a, int CWp, int EXp, int num)
//(course, grading, student, course the student taking)
{
    int call=1;
    double score;
    stringstream linestream;
    string line, section;
    char test;

    fcourse >> test;
    while (test!='*')
    {
        linestream.clear();
        getline (fcourse, line, ',');
        //cout << test << line << " ";
        linestream << line;
        if (line>="a" && line<="z")
            a.name=(test+line);
        else if (line.length()==9 && !(test>='a' && test<='z'))
            a.uid=(test+line);
        else if (test=='E')
        {
            for (int phrase0=0; phrase0<call; phrase0++)
                linestream >> section;
            linestream >> a.EX[num];
        }
        else if (test=='C')
        {
            for (int phrase1=0; phrase1<call; phrase1++)
                linestream >> section;
            linestream >> a.CW[num];
        }
        fcourse >> test;
    }
    cout << a.name << " [" << a.uid << "]" << endl;
    score = (a.CW[num]/100.0*CWp)+(a.EX[num]/100.0*EXp);
    return readGrade(fgrade, score);
}

bool findStudent (ifstream &fcourse, Student a)
//to see if the student attends the course.
{
    string text;
    fcourse.clear();
    fcourse.seekg(0,ios::beg);
    while (fcourse.good())
    {
        getline (fcourse, text);
        int pos = text.find (a.uid);
        if (pos!=-1)        //if found name of student in the file, num++
            return true;
    }
    return false;
}

void displayOne (ifstream &fcourse, ifstream &fgrade, string Cname, Student a, int CWp, int EXp, int i)
//(course[i], grade, student a, the class index the student is taking.
{
    string text;
    a.gpa[i] = readCourse(fcourse, fgrade, a, CWp, EXp, i);
    if (findStudent(fcourse, a)==true)
        cout << Cname << ": " << a.gpa[i] << endl << endl;
}

void displayAll (ifstream &fcourse, ifstream &fgrade)  //(fdata, f);
//open the course file-> get the info of students-> read score-> obtain gpa
//->display name, uid, course, gpa
{
    Student a[300];

    while (fcourse.good())
    {
        //for (int i=0; i<300; i++)
            //for (int j=0; j<6; j++)
                //displayOne (fcourse, fgrade, a[i], j);
    }
}

void searchKey (ifstream &fin)
{
    string key;

    getline (cin, key);
}

void Top (ifstream &fin, int top)
{

}

int main()
{
    Student s[300];
    ifstream fdata, fgrade, fCourse[12];
    string command, course[12], cCode[12];;
    char dummy, Cname[14];
    int index=0, CWp[12], EXp[12], a=-1;
    //int t;

    fdata.open ("data.txt");
    fgrade.open ("grade.txt");

    error (fdata);
    error (fgrade);

    while (fdata.good()) //ok
    {
        fdata>>Cname>>dummy>>dummy>>CWp[index]>>dummy>>dummy>>EXp[index];
        course[index]=Cname;
        cCode[index]=course[index].substr(0, 8);
        fCourse[index].open (course[index].c_str());
        error (fCourse[index]);
        index++;
    }

    cout << "all | search | top | <uid> | end" << endl;
    getline(cin, command);

    while (command != "end")
    {
        if (command == "all")
        {
            displayOne(fCourse[0], fgrade, cCode[0], s[0], CWp[0], EXp[0], 0);
            //for (int i=0; i<(index--); i++)
                //for (int j=0; j<300; j++)
                    //displayOne(fCourse[i], fgrade, s[0], a);
        }

        if (command == "search")
        {
            //searchKey ()
        }

        //if (command == "top"+t)
            //Top (fCourse, t);

        //if (command == uid)
        cout << endl << "all | search | top | <uid> | end" << endl;
        getline(cin, command);
    }
    for (int i=0; i<(index--); i++)
        fCourse[i].close();
    fdata.close();
    fgrade.close ();
    return 0;
}
