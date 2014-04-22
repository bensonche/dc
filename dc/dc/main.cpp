#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

#define DATA_FILE "data.txt"
#define GRADE_FILE "grade.txt"

using namespace std;


struct Grade
{
    string course;
    double gpa;
};

struct GradeList
{
    Grade **list;
    int count;
    
    GradeList()
    {
        count = 0;
    }
};

struct Student
{
    string name, uid;
    GradeList g;
};

struct StudentList
{
    Student **list;
    int count;
    
    StudentList()
    {
        list = new Student*[300];
        count = 0;
    }
};

struct CourseData
{
    string uid;
    int cw;
    int ex;
    string name;
};

struct CourseDataList
{
    CourseData **list;
    int count;
    
    CourseDataList()
    {
        list = new CourseData*[100];
        count = 0;
    }
};

struct Course
{
    string name;
    int cw;
    int ex;
    CourseDataList *data;
};

struct CourseList
{
    Course **list;
    int count;
    
    CourseList()
    {
        list = new Course*[6];
        count = 0;
    }
};

void addStudent(StudentList *list, Student *s)
{
    list->list[list->count++] = s;
}

void addGrade(GradeList *list, Grade *g)
{
    list->list[list->count++] = g;
}

void addCourse(CourseList *list, Course *c)
{
    list->list[list->count++] = c;
}

void addCourseData(CourseDataList *list, CourseData *c)
{
    list->list[list->count++] = c;
}

void printCourseDataList(CourseDataList *list)
{
    for(int i = 0; i < list->count; i++)
    {
        cout << list->list[i]->uid << endl;
        cout << list->list[i]->cw << endl;
        cout << list->list[i]->ex << endl;
        cout << list->list[i]->name << endl;
		cout << endl;
    }
}

void error (ifstream &fin)
{
    if (fin.fail())
    {
        cout << "Error opening file" << endl;
        exit (1);
    }
}

// Read files
CourseDataList* readSpecificCourse(string name)
{
    ifstream f;
    f.open(name + ".txt");
    error(f);
    
    stringstream linestream, iss;
    string temp, line, tok;
    
    CourseDataList *list = new CourseDataList;
    CourseData *c = new CourseData;
    
    while(getline(f, line))
    {
        linestream.clear();
        linestream << line;
        
        while(getline(linestream, tok, ','))
        {
            if (tok.length() == 10 && !(tok[0] >= 'a' && tok[0] <= 'z')) 
                c->uid = tok;
            else if (tok[0] == 'E')
            {
                iss.clear();
                iss << tok.substr(3, 2);
                iss >> c->ex;
            }
            else if (tok[0]=='C')
            {
                iss.clear();
                iss << tok.substr(3, 2);
                iss >> c->cw;
            }
            else if(tok[0] != '*')
				c->name = tok;
        }
        
        addCourseData(list, c);
        c = new CourseData;
    }
    
    f.close();
    return list;
}

CourseList* readData()
{
    ifstream fdata;
    fdata.open(DATA_FILE);
    error(fdata);
    
    char dummy;
    
    CourseList *list = new CourseList[6];
    
    while (fdata.good())
    {
        Course *c = new Course;
        string filename;
        
        fdata >> filename >> dummy >> dummy >> c->cw >> dummy >> dummy >> c->ex;
        c->name = filename.substr(0, filename.length() - 4);
        
        c->data = readSpecificCourse(c->name);
        
        printCourseDataList(c->data);
        
        addCourse(list, c);
    }
    
    fdata.close ();
    return list;
}

double readGrade (double score) //ok
//determine the gpa of student in that course.
{
    ifstream fgrade;

    fgrade.open ("grade.txt");
    error (fgrade);

    int LS, HS;
    double grade;
    char dummy;
    fgrade.clear ();
    fgrade.seekg (0, ios::beg);
    while (fgrade.good())
    {
        fgrade >> LS >> dummy >> HS >> dummy >> grade;
        if (score>LS && score<HS)
        {
            fgrade.close();
            return grade;
        }
    }
    fgrade.close ();
    return 99.0;
}

/*

double readCourse (ifstream &fcourse, Student a, int CWp, int EXp, int cNum)
//(course, grading, student, course the student taking)
{
    int call=1;
    double score, CW, EX;
    stringstream linestream;
    string line, section;
    char test;

    fcourse >> test;
    while (test!='*')
    {
        linestream.clear();
        getline (fcourse, line, ',');
        //cout << test << line << endl;
        linestream << line;
        if (line>="a" && line<="z")
            a.name=(test+line);
        else if (line.length()==9 && !(test>='a' && test<='z'))
            a.uid=(test+line);
        else if (test=='E')
        {
            for (int phrase0=0; phrase0<call; phrase0++) 
                linestream >> section;
            linestream >> EX;
        }
        else if (test=='C')
        {
            for (int phrase1=0; phrase1<call; phrase1++)
                linestream >> section;
            linestream >> CW;
        }
        fcourse >> test;
    }
    cout << a.name << " [" << a.uid << "]" << endl;
    score = (CW/100.0*CWp)+(EX/100.0*EXp);
    return readGrade(score);
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

void displayOne (ifstream &fcourse, string Cname, Student a, int CWp, int EXp, int cNum)
//(course[i], grade, student a, the class index the student is taking.
{
    string text;
    a.gpa[cNum] = readCourse(fcourse, a, CWp, EXp, cNum);
    if (findStudent(fcourse, a)==true)
        cout << Cname << ": " << a.gpa[cNum] << endl << endl;
}

void sorting (StudentList s)
//to rearrange the display orders with ascending alphabets of student's name.
{
    Student temp;
    for (int i=0; i<s.Count; i++)
        for (int j=(i+1); j<s.Count; j++)
        {
            if (s.List[j].name<s.List[i].name)
            {
                temp=s.List[j];
                s.List[j]=s.List[i];
                s.List[i]=temp;
            }
        }
}

void displayAll (ifstream &fcourse)  //(fdata, f);
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

void searchKey (StudentList s, string key)
//search the name/uid of student in the StudentList that that contains the string key.
{


}

void Top (StudentList s, int top)
//display student that has the highest average gpa.
//'top' is the number of top students the user want to show.
{

}
*/
void prompt()
{
	string command, uid;

	cout << "all | search | top | <uid> | end" << endl;
    getline(cin, command);

    while (command != "end")
    {
        if (command == "all")
        {
            //for (int i=0; i<(index--); i++)

                    //displayOne(fCourse[i], fgrade, s[0], a);
        }

        if (command == "search")
        {
			;
            //getline (cin, key);
            //searchKey (s, key);

        }

        if (command == "top")
			;
            //Top (s, topNum);

   //     if (command >> uid)
   //     {
			//;
   //         //UIDstream << uid << endl;
   //         //spStudent.uid=UIDstream.str();
   //     }
        cout << endl << "all | search | top | <uid> | end" << endl;
        getline(cin, command);
    }
}

int main()
{
    CourseList *list = readData();

	prompt();
    /*
    Student a; //spStudent;
    StudentList s;
    ifstream fdata, fCourse[12];
    string command, course[12], cCode[12];//, key;
    //stringstream UIDstream;
    char dummy, Cname[14];
    int index=0, CWp[12], EXp[12], num=0; //, uid, topNum;

    fdata.open ("data.txt");

    error (fdata);

    while (fdata.good()) //ok
    {
        fdata>>Cname>>dummy>>dummy>>CWp[index]>>dummy>>dummy>>EXp[index];
        course[index]=Cname;
        cCode[index]=course[index].substr(0, 8);
        fCourse[index].open (course[index].c_str());
        error (fCourse[index]);
        index++;
    }

    for (int j=0; j<=s.Count; j++)      //problem here lol, can't run program
    {
        for (int i=0; i<index; i++)         //read all the data from all the course files.
        {
            if (findStudent(fCourse[index], s.List[j])==true)
            {
                s.List[j].course[num]=cCode[i];
                s.List[j].gpa[num]=readCourse (fCourse[i], s.List[j], CWp[i], EXp[i], i);
                num++;
            }
        }
        s.Count++;
    }


    cout << "all | search | top | <uid> | end" << endl;
    getline(cin, command);

    while (command != "end")
    {
        if (command == "all")
        {
            for (int j=0; j<300; j++)
                for (int i=0; i<(index--); i++)
                    displayOne(fCourse[i], cCode[i], s.List[j], CWp[i], EXp[i], i);
            //for (int i=0; i<(index--); i++)

                    //displayOne(fCourse[i], fgrade, s[0], a);
        }

        //if (command == "search")
        {
            //getline (cin, key);
            //searchKey (s, key);

        }

        //if (command == "top")
            //Top (s, topNum);

        //if (command >> uid)
        {
            //UIDstream << uid << endl;
            //spStudent.uid=UIDstream.str();
        }
        cout << endl << "all | search | top | <uid> | end" << endl;
        getline(cin, command);
    }
    while (index>0)
        fCourse[index--].close();
    fdata.close();
    */
    return 0;
}
