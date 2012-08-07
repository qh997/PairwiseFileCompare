#ifndef MYDEF_H
#define MYDEF_H

//#define DEBUG

#define TRUE (1)
#define FALSE (0)
#define OK (1)
#define ERROR (0)
#define INFEASIBLE (-1)
#define OVERFLOW (-2)
typedef int Status;

#ifdef DEBUG
#define show_var(fmt, var) printf("%s = "fmt"\n", #var, var)
#define show_log(var) printf("%s\n", var);
#else
#define show_var(fmt, var)
#define show_log(var)
#endif

#endif //MYDEF_H