#include <stdio.h>
#include <windows.h>

//Declaracion de variables Globales
HHOOK hookMouseS;

void displayMensajes(){
  MSG mensaje;
  while(GetMessage(&mensaje, NULL, 0, 0)){
      TranslateMessage(&mensaje);
      DispatchMessage(&mensaje);
  }
}

int keyStroke(int iKey){
  return (GetAsyncKeyState(iKey) & 0x8000 != 0);
}

//Funcion para desplegar y guardar el registro en el archivo
void printAndSaveInformation(int X, int Y){
  //Declaracion de variables
  FILE *outputFile = fopen("outputFile.txt", "ab");
  //Despelgar informacion y guardar en archivo
  printf("Clic detectado! X=%d Y=%d\n",X,Y);
  fprintf(outputFile,"Clic detectado! X=%d Y=%d\n",X,Y);
  //Cerramos el archivo
  fclose(outputFile);
}

//Funcion para detectar los clics del mouse en el desktop
__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int codigo, WPARAM parametrosW, LPARAM parametrosL){
  //Declaracion de variables
  MOUSEHOOKSTRUCT *estructuraMouse = parametrosL;
  //Ver si no es null (indicaria error) y si tenemos precionada una tecla
  if (estructuraMouse != NULL && keyStroke(VK_LBUTTON)){
      printAndSaveInformation(estructuraMouse -> pt.x,estructuraMouse -> pt.y);
  }
  return CallNextHookEx(hookMouseS, codigo, parametrosW, parametrosL);
}

//Funcion del loger para el mouse
DWORD WINAPI mouseLogger(LPVOID lp){
  //Declaracion de variables
  HINSTANCE instancia = GetModuleHandle(NULL);
  if(!instancia){
        instancia = LoadLibrary((LPCSTR) lp);
        if(!instancia){
          //salir si no se pudo cargar la libreria
          return 1;
        }
  }
  //Agregar el hook
  hookMouseS = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC) KeyboardEvent, instancia,NULL);
  //Mandar llamar las funciones del mensaje
  displayMensajes();
  UnhookWindowsHookEx(hookMouseS);
  return 0;
}
//Declaracion de la funcion MAIN
int main(int argc, char *argv[]){
  //Declaracion de variables
  HANDLE threadHandler;
  DWORD threadWord;
  //Creamos el thread
  threadHandler = CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)mouseLogger,(LPVOID)argv[0],NULL,&threadWord);
  while (threadHandler){
      return WaitForSingleObject(threadHandler,INFINITE);
  }
  return 0;
}