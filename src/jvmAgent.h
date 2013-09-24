// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the JVMAGENT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// JVMAGENT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef JVMAGENT_EXPORTS
#define JVMAGENT_API __declspec(dllexport)
#else
#define JVMAGENT_API __declspec(dllimport)
#endif

#include <jni.h>
#include <jvmti.h>
#include <string.h>
#include <list>
#include <string>

using std::list;
using std::string;


class CjvmAgent {
public:
    CjvmAgent(void);
};

/** Metodo chamado na carga do agente*/
JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved);

/** Metodo chamado na descarga do agente*/
JNIEXPORT void JNICALL
Agent_OnUnLoad(JavaVM *vm);

static jrawMonitorID agent_lock;


/* Callbacks */
static void JNICALL cbVMStart(jvmtiEnv *jvmti, JNIEnv *env);

static void JNICALL cbVMInit(jvmtiEnv *jvmti, JNIEnv *env, jthread thread);

static void JNICALL cbVMDeath(jvmtiEnv *jvmti, JNIEnv *env);

static void JNICALL cbObjectFree(jvmtiEnv *jvmti, jlong tag);

static void JNICALL cbVMObjectAlloc(jvmtiEnv *jvmti, JNIEnv *env, jthread thread,
                                    jobject object, jclass object_klass, jlong size);

static void JNICALL cbMethodEntry(jvmtiEnv *jvmti, JNIEnv* env,
                                  jthread thread, jmethodID method);

static void JNICALL cbMethodExit(jvmtiEnv *jvmti, JNIEnv* env,
                                 jthread thread, jmethodID method,
                                 jboolean was_popped_by_exception,
                                 jvalue return_value);

static void JNICALL
cbClassFileLoadHook(jvmtiEnv *jvmti, JNIEnv* env,
                    jclass class_being_redefined, jobject loader,
                    const char* name, jobject protection_domain,
                    jint class_data_len, const unsigned char* class_data,
                    jint* new_class_data_len,
                    unsigned char** new_class_data);

/* Estruturas auxiliares */
typedef struct {
  int lastStart;
  int lastEnd;
  int invokes;
  string *name;
} MethodInfo;

/* Funções auxiliares */
list<string *> getMostCalledMethods();
