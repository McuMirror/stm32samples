#ifndef _U_
#define _U_ __attribute__((__unused__))
#endif

#define CMD_MAXLEN  (32)

enum{
   RET_CMDNOTFOUND = -2,
   RET_WRONGCMD = -1,
   RET_BAD = 0,
   RET_GOOD = 1
};

int parsecmd(const char *cmdwargs);

#define CMD_ABSPOS          (3056382221)
#define CMD_ACCEL           (1490521981)
#define CMD_ADC             (2963026093)
#define CMD_BUTTON          (1093508897)
#define CMD_CANERRCODES     (1736697870)
#define CMD_CANFILTER       (3964416573)
#define CMD_CANFLOOD        (1235816779)
#define CMD_CANFLOODT       (506574623)
#define CMD_CANID           (2040257924)
#define CMD_CANIGNORE       (3209755195)
#define CMD_CANINCRFLOOD    (3987155959)
#define CMD_CANPAUSE        (3981532373)
#define CMD_CANREINIT       (2030075842)
#define CMD_CANRESUME       (2051659720)
#define CMD_CANSEND         (237136225)
#define CMD_CANSPEED        (549265992)
#define CMD_CANSTAT         (237384179)
#define CMD_DELIGNLIST      (2235834164)
#define CMD_DIAGN           (2334137736)
#define CMD_DUMPCMD         (1223955823)
#define CMD_DUMPCONF        (3271513185)
#define CMD_DUMPERR         (1223989764)
#define CMD_EMSTOP          (2965919005)
#define CMD_ENCPOS          (3208428301)
#define CMD_ENCREV          (3208460296)
#define CMD_ENCSTEPMAX      (1022989757)
#define CMD_ENCSTEPMIN      (1022990779)
#define CMD_ERASEFLASH      (3177247267)
#define CMD_ESW             (2963094612)
#define CMD_ESWREACT        (1614224995)
#define CMD_GOTO            (4286309438)
#define CMD_GOTOZ           (3178103736)
#define CMD_GPIO            (4286324660)
#define CMD_GPIOCONF        (1309721562)
#define CMD_MAXSPEED        (1498078812)
#define CMD_MAXSTEPS        (1506667002)
#define CMD_MCUT            (4022718)
#define CMD_MCUVDD          (2517587080)
#define CMD_MICROSTEPS      (3974395854)
#define CMD_MINSPEED        (3234848090)
#define CMD_MOTFLAGS        (2153634658)
#define CMD_MOTMUL          (1543400099)
#define CMD_MOTREINIT       (199682784)
#define CMD_PING            (10561715)
#define CMD_RELPOS          (1278646042)
#define CMD_RELSLOW         (1742971917)
#define CMD_RESET           (1907803304)
#define CMD_SAVECONF        (141102426)
#define CMD_SCREEN          (2100809349)
#define CMD_SPEEDLIMIT      (1654184245)
#define CMD_STATE           (2216628902)
#define CMD_STOP            (17184971)
#define CMD_TIME            (19148340)
#define CMD_TMCBUS          (1906135955)
#define CMD_UDATA           (2736127636)
#define CMD_USARTSTATUS     (4007098968)