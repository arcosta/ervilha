// jvmAgent.cpp : Defines the exported functions for the DLL application.

#include "jvmAgent.h"
#include <stdlib.h>
#include <sys/time.h>
#include <map>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>


CjvmAgent::CjvmAgent()
{
    return;
}

/*Methods list */
std::map<jmethodID, MethodInfo *> methodsList;

static int port = 8282; //FIXME: Isso aqui deveria vir no arquivo de configuracao

void parse_options(char *opt) {
    printf("Parsing parameters list\n");

    //TODO: Usar o strtoken ou a biblioteca getopt para fazer o parsing
}

JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved)
{
    printf("Agent loaded successfully\n");

    jvmtiEnv *jvmti		= NULL;
    jvmtiError error	= JVMTI_ERROR_NONE;
    jint res			= 0;
    jvmtiCapabilities capabilities;
    jvmtiEventCallbacks callbacks;

    res = vm->GetEnv((void **) &jvmti, JVMTI_VERSION_1_0);
    if (res != JNI_OK) {
        printf("Error creating jvmti environment\n");
        return res;
    }

    //TODO: Parse de opções passados
    parse_options(options);

    // Limpa a estrutura de capacidades e ativas as interessantes
    (void)memset(&capabilities,0, sizeof(capabilities));
    capabilities.can_generate_all_class_hook_events  = 1;
    capabilities.can_tag_objects                     = 1;
    capabilities.can_generate_object_free_events     = 1;
    capabilities.can_get_source_file_name            = 1;
    capabilities.can_get_line_numbers                = 1;
    capabilities.can_generate_vm_object_alloc_events = 1;
    capabilities.can_generate_method_entry_events    = 1;
    capabilities.can_generate_method_exit_events     = 1;

    // Ativas as capabilities
    error = jvmti->AddCapabilities(&capabilities);
    if (error != JVMTI_ERROR_NONE) {
        printf("Error on capabilities activation\n");
        return error;
    }

    // Limpa a estrutura de callbacks e ativas as interessantes
    (void)memset(&callbacks,0, sizeof(callbacks));
    callbacks.VMStart           = &cbVMStart;
    callbacks.VMInit            = &cbVMInit;
    callbacks.VMDeath           = &cbVMDeath;
    callbacks.ObjectFree        = &cbObjectFree;
    callbacks.VMObjectAlloc     = &cbVMObjectAlloc;
    callbacks.ClassFileLoadHook = &cbClassFileLoadHook;
    callbacks.MethodEntry	= &cbMethodEntry;
    callbacks.MethodExit	= &cbMethodExit;
    error = jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));
    if (error != JVMTI_ERROR_NONE) {
        printf("Error on callbacks activation\n");
        return error;
    }

    // Para cada callback deve ser habilitado um evento.
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_VM_START, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_VM_INIT, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_VM_DEATH, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_OBJECT_FREE, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_VM_OBJECT_ALLOC, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_METHOD_ENTRY, (jthread)NULL);
    error = jvmti->SetEventNotificationMode(JVMTI_ENABLE,
                                            JVMTI_EVENT_METHOD_EXIT, (jthread)NULL);
    if (error != JVMTI_ERROR_NONE) {
        printf("Error on events enabling\n");
        return error;
    }

    // cria um monitor para seções criticas.
    error = jvmti->CreateRawMonitor("agent data", &(agent_lock));
    if (error != JVMTI_ERROR_NONE) {
        printf("Error creating monitor\n");
        return error;
    }
    
    
          
    return JNI_OK;
}

JNIEXPORT void JNICALL
Agent_OnUnLoad(JavaVM *vm)
{
    jvmtiEnv *jvmti;
    vm->GetEnv((void **) &jvmti, JVMTI_VERSION_1_0);
    
    //TODO: esse print nao eh chamado
    printf("\nAgent unloaded\n");
}

/* --------------------------------------------------------------- */
// Implementação dos callbacks
/* --------------------------------------------------------------- */

// Chamado quando a JVM é iniciada
static void JNICALL cbVMStart(jvmtiEnv *jvmti, JNIEnv *env)
{
}

//TODO: Qual a diferença entre o Start e o Init
// Chamado quando a JVM é inicializada
static void JNICALL cbVMInit(jvmtiEnv *jvmti, JNIEnv *env, jthread thread)
{
}

// Chamado quando a JVM é finalizada
static void JNICALL cbVMDeath(jvmtiEnv *jvmti, JNIEnv *env)
{
}

//FIXME: Acho que é usando quando o objeto é coletado
static void JNICALL cbObjectFree(jvmtiEnv *jvmti, jlong tag)
{
}

// Chamado quando um objeto é alocado
static void JNICALL cbVMObjectAlloc(jvmtiEnv *jvmti, JNIEnv *env, jthread thread,
                                    jobject object, jclass object_klass, jlong size)
{
  //TODO: Podemos começar daqui a implementar a verificação dos maiores objetos na memória
}

static void JNICALL cbMethodEntry(jvmtiEnv *jvmti, JNIEnv* env, jthread thread, jmethodID method)
{
  /**
   * TODO: Do jeito que está, estamos armazenando o tempo da ultima execução. Seria interessante salvar a 
   *execucao mais demorada
   */
    int retval;
    struct timeval tv;
    MethodInfo *mi = new MethodInfo();
    std::map<jmethodID, MethodInfo *>::iterator it = methodsList.find(method);
    
    retval = gettimeofday(&tv, NULL);
    if(retval == -1){
      return;
    }
    
    if(it != methodsList.end()){
      it->second->invokes++;
      it->second->lastStart = tv.tv_usec;
    } else {
      mi->invokes = 1;
      mi->lastStart = tv.tv_usec;
      methodsList.insert(std::pair<jmethodID, MethodInfo *>(method, mi));
    }
}

static void JNICALL cbMethodExit(jvmtiEnv *jvmti, JNIEnv* env, jthread thread,
                                 jmethodID method, jboolean was_popped_by_exception, jvalue return_value)
{
    char *name;
    jvmtiError error = JVMTI_ERROR_NONE;
    struct timeval tv;
    jclass classe;
    char *classname;
    
    gettimeofday(&tv, NULL);
    
    std::map<jmethodID, MethodInfo *>::iterator it = methodsList.find(method);
    if(it != methodsList.end()){
      it->second->lastEnd = tv.tv_usec;
    } else {
      return;
    }
        
    error = error + jvmti->GetMethodName(method, &name, NULL, NULL);
    error = error + jvmti->GetMethodDeclaringClass(method, &classe);
    error = error + jvmti->GetClassSignature(classe, &classname, NULL);
    if(error != JVMTI_ERROR_NONE){
      printf("Error gathering method information\n");
      return;
    } else {
      it->second->name = new string(name);
    }
    
    /*
    printf("Metodo (%li) finalizado: %s em %i ms", (long) method, name, (it->second->lastEnd - it->second->lastStart));
    printf("\tChamado %i vezes", it->second->invokes);
    printf(" classe: %s\n", classname);
    */
}

static void JNICALL
cbClassFileLoadHook(jvmtiEnv *jvmti, JNIEnv* env,
                    jclass class_being_redefined, jobject loader,
                    const char* name, jobject protection_domain,
                    jint class_data_len, const unsigned char* class_data,
                    jint* new_class_data_len,
                    unsigned char** new_class_data)
{
    //enterCriticalSection(jvmti);
    {
        //if(!gdata->vmDead) {
        const char *classname;
        if (name == NULL) {
            classname = "**\0";
        } else {
            classname = strdup(name);
        }

        (void)free((void*)classname);
        //}
    }//exitCriticalSection(jvmti);
}

/**
 * Lista os metodos mais chamados
 */
list<string *> getMostCalledMethods()
{
  list<string *> retList;
  std::map<jmethodID, MethodInfo *>::iterator it;
  
  for(it = methodsList.begin(); it != methodsList.end();++it)
  {
    retList.push_back(it->second->name);
  }
  return retList;
}
