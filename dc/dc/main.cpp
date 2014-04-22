#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <sstream>

#define DATA_FILE "data.txt"
#define GRADE_FILE "grade.txt"

using namespace std;

double getGpa (int courseCW, int courseEX, int CW, int EX);
double readGrade (double score);

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
		list = new Grade*[6];
        count = 0;
    }
};

struct Student
{
    string name, uid;
    GradeList *gradeList;

	Student()
    {
		gradeList = new GradeList;
    }
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

void addGrade(GradeList *list, Grade *g)
{
    list->list[list->count++] = g;
}

void addCourse(CourseList *list, Course *c)
{
    list->list[list->count++] = c;
}

// Compare by name first, then uid
int compareStudents(Student *a, Student *b)
{
    int result = strcmp(a->name.c_str(), b->name.c_str());
    if(result == 0)
        return strcmp(a->uid.c_str(), b->uid.c_str());
        
    return result;
}

// Returns index of student. Otherwise return -1 if not found
int findStudent(StudentList *list, Student *s, int min, int max)
{
    if (min > max || list->count == 0)
        return -1;
    if(min == max)
    {
        if(compareStudents(list->list[min], s) == 0)
			return min;
        else
			return -1;
    }
	if(min + 1 == max)
    {
		if(compareStudents(list->list[min], s) == 0)
			return min;
		if(compareStudents(list->list[max], s) == 0)
			return max;
		return -1;
    }
    
    int mid = (max + min) / 2.0;

    int result = compareStudents(list->list[mid], s);
    if(result == 0)
    {
        return mid;
    }
    if(result > 0)
        return findStudent(list, s, min, mid);
    return findStudent(list, s, mid, max);
}

void addStudent(StudentList *list, Student *s)
{
    if(list->count == 0)
        list->list[list->count++] = s;
    else
    {
        int index = findStudent(list, s, 0, list->count - 1);
        if(index == -1) // Insert into the list
        {
            for(int i = list->count; i > 0; i--)
            {
                if(compareStudents(list->list[i-1], s) > 0)
                {
                    list->list[i] = list->list[i-1];
                }
                else
                {
                    list->list[i] = s;
                    list->count++;
                    return;
                }
            }
        }
        else // Update existing student
        {
            Grade *g = new Grade;
            g->course = s->gradeList->list[0]->course;
			g->gpa = s->gradeList->list[0]->gpa;

			addGrade(list->list[index]->gradeList, g);

			delete s->gradeList->list[0];
			delete [] s->gradeList->list;
			delete s->gradeList;
			delete s;
        }
    }
}

void addCourseData(CourseDataList *list, CourseData *c)
{
    list->list[list->count++] = c;
}

void printStudentList(StudentList *list)
{
    for(int i = 0; i < list->count; i++)
    {
        Student *s = list->list[i];
		cout << s->name << " [" << s->uid << "]" << endl;

		GradeList *grades = s->gradeList;
		for(int j=0; j<grades->count; j++)
        {
			Grade *grade = grades->list[j];
			cout << grade->course << ": " << grade->gpa << endl;
        }
		
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
CourseDataList* readSpecificCourse(Course *course, StudentList *students)
{
    ifstream f;
    f.open((course->name + ".txt").c_str());
    error(f);
    
    stringstream linestream, iss;
    string temp, line, tok;
    
    CourseDataList *list = new CourseDataList;
    CourseData *c;
    
    while(getline(f, line))
    {
        c = new CourseData;
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

		Student *s = new Student;
		s->name = c->name;
		s->uid = c->uid;
		Grade *g = new Grade;
		g->course = course->name;
        g->gpa = getGpa(course->cw, course->ex, c->cw, c->ex);
		addGrade(s->gradeList, g);

		addStudent(students, s);
    }
    
    f.close();
    return list;
}

CourseList* readData(StudentList *students)
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
        
        c->data = readSpecificCourse(c, students);

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

double getGpa (int courseCW, int courseEX, int CW, int EX)
{
    double score;
    score = (CW/100.0*courseCW)+(EX/100.0*courseEX);
    return readGrade(score);
}

void prompt(StudentList *students)
{
	string command, uid;

	cout << "all | search | top | <uid> | end" << endl;
    getline(cin, command);

    while (command != "end")
    {
        if (command == "all")
        {
			printStudentList(students);
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
    StudentList *students = new StudentList;
    CourseList *list = readData(students);

	prompt(students);

    return 0;
}
