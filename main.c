/*
Guilherme Salgado Alves
Jefferson Vieira França
Rafael Miranda
*/

#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  float x, y, z;
  bool cor;
  float dx;
  bool bot;
} Ponto_t, *Ponto;

typedef struct {
  Ponto_t *ponto;
  size_t used;
  size_t size;
} Array;

float amarelo[] = {1, 1, 0}, azul[] = {0, 0, 1}, branco[] = {1, 1, 1},
      ciano[] = {0, 1, 1}, verde[] = {0, 1, 0}, vermelho[] = {1, 0, 0},
      cinza[] = {.54, .54, .54};

double theta = 90, aspect = 1, d_near = 1, d_far = 1800;
double x_0 = 0, y_0 = 40.0, z_0 = -100, x_ref = 0, y_ref = 0, z_ref = -200,
       V_x = 0, V_y = 1, V_z = 0;

bool ligado = false, colidiu = false;
int contaCor = 0, voltaAnt = 0;
float posCeu = 0, R = 1, G = 1, B = 0;

int tamPista = 19000, larguraPista = 80, volta = 0;

int pos = 0;
float playerPos = 0.0, speed = 1;

int botPos = 500;
float *corBot[] = {amarelo, verde, azul, ciano};

int sw, sh, winPosX, winPosY;
float w_width, w_height;
int posQndoBateu, voltaAnt2, contaCor2;
float ambiente[5], posicao[4];

Array Pontos;
float x, dx;

int contador, dxBot;

void initArray(Array *a, size_t initialSize) {
  a->ponto = (Ponto_t *)calloc(initialSize, sizeof(Ponto_t));
  a->used = 0;
  a->size = initialSize;
}
void insertArray(Array *a, Ponto_t element) {
  if (a->used == a->size) {
    a->size *= 2;
    a->ponto = (Ponto_t *)realloc(a->ponto, a->size * sizeof(Ponto_t));
  }
  a->ponto[a->used++] = element;
}

void desenhaCarro(float *cor) {
  glPushMatrix();
  glColor3fv(cor);
  glutSolidCube(10.0);
  glPopMatrix();
}

void initScreen() {
  w_width = glutGet(GLUT_WINDOW_WIDTH);
  w_height = glutGet(GLUT_WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(theta, aspect, d_near, d_far);
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(x_0, y_0, z_0, x_ref, y_ref, z_ref, V_x, V_y, V_z);
  glPopMatrix();
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambiente);

  // iluminacao da pista
  if (volta != voltaAnt2) {
    voltaAnt2 = volta;
    contaCor2 = (contaCor2 + 1) % 4;
  }
  if (contaCor2 == 3) {
    ambiente[0] = 1.0 - B;
    ambiente[1] = 1.0 - B;
    ambiente[2] = 1.0 - B;
    ambiente[4] = 1.0;
  } else if (contaCor2 == 2) {
    ambiente[0] = 1.0;
    ambiente[1] = 1.0;
    ambiente[2] = 1.0;
    ambiente[4] = 1.0;
  } else if (contaCor2 == 1) {
    ambiente[0] = B;
    ambiente[1] = B;
    ambiente[2] = B;
    ambiente[4] = 1.0;
  } else {
    ambiente[0] = 0.0;
    ambiente[1] = 0.0;
    ambiente[2] = 0.0;
    ambiente[4] = 1.0;
  }
}
void fitWindow(GLsizei w, GLsizei h) {
  if (w >= h)
    glutReshapeWindow(h, h);
  else
    glutReshapeWindow(w, w);
}

void reshape(int w, int h) {
  if (w >= h)
    glViewport(0, 0, (GLsizei)h, (GLsizei)h);
  else
    glViewport(0, 0, (GLsizei)w, (GLsizei)w);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  initScreen();
}

int tocandoEsquerda() { return playerPos >= -(larguraPista / 2 + 30) ? 0 : 1; }

int tocandoDireita() { return playerPos <= larguraPista / 2 + 30 ? 0 : 1; }

void desenhaCenario(float *cor, float x1, float y1, float z1, float x2,
                    float y2, float z2, float w) {
  glColor3fv(cor);
  glBegin(GL_QUADS);
  glVertex3f(x1 - w, y1, z1);
  glVertex3f(x2 - w, y2, z2);
  glVertex3f(x2 + w, y2, z2);
  glVertex3f(x1 + w, y1, z1);
  glEnd();
}

void timerFunc(int valor) {
  int f = valor;
  if (colidiu) {
    if (posQndoBateu > 100) {
      colidiu = false;
    } else {
      pos = pos - 0.5 * (speed / 10);
      speed = 2;
      botPos += 2;
      posQndoBateu++;
    }
  } else {
    pos += speed;
  }
  botPos += 0.8 * speed;

  while (pos >= tamPista) {
    pos -= tamPista;
    volta++;
  }
  while (pos < 0)
    pos += tamPista;
  while (botPos >= tamPista)
    botPos -= tamPista;
  while (botPos < 0)
    botPos += tamPista;

  // iluminacao do ceu
  if (volta != voltaAnt) {
    voltaAnt = volta;
    contaCor = (contaCor + 1) % 4;
  }
  if (contaCor == 3) {

    B = ((float)(pos + 0.1) / (float)tamPista);
    R = 2 * B;
    G = 2 * B;
    glClearColor(.0f, .0f, 1.0 - B, .0f);
  } else if (contaCor == 2) {
    glClearColor(.0f, .0f, 1.0, .0f);
  } else if (contaCor == 1) {
    B = ((float)pos / (float)tamPista);
    R = 1;
    G = 1;
    glClearColor(.0f, .0f, B, .0f);
  } else {
    glClearColor(.0f, .0f, 0, .0f);
  }

  // verifica se o carro está tocando alguma das bordas e desacelera
  if (tocandoDireita() || tocandoEsquerda()) {
    pos -= (0.12 * speed);
    botPos += 0.15 * speed;
    speed -= 0.02;
  }
  initScreen();
  if (ligado)
    glutTimerFunc(5, timerFunc, f);
  glutPostRedisplay();
}

void desenhaPista() {
  Ponto_t *p2, *p1;
  int n;
  for (n = pos; n < pos + 2500; n++) {
    p1 = &(Pontos.ponto[(n - 1) % tamPista]);
    p2 = &(Pontos.ponto[n % tamPista]);
    p2->x = x;
    desenhaCenario(verde, p1->x, p1->y - 2,
                   p1->z + pos - (n - 1 >= tamPista ? tamPista : 0), p2->x,
                   p2->y - 2, p2->z + pos - (n >= tamPista ? tamPista : 0),
                   larguraPista * 200);
    desenhaCenario(branco, p1->x, p1->y - 1,
                   p1->z + pos - (n - 1 >= tamPista ? tamPista : 0), p2->x,
                   p2->y - 1, p2->z + pos - (n >= tamPista ? tamPista : 0),
                   larguraPista * 1.2);
    desenhaCenario(cinza, p1->x, p1->y,
                   p1->z + pos - (n - 1 >= tamPista ? tamPista : 0), p2->x,
                   p2->y, p2->z + pos - (n >= tamPista ? tamPista : 0),
                   larguraPista);
  }
  glutPostRedisplay();
}

void desenhaBot(float *cor, int dzBot, int dx) {
  if ((botPos + dzBot) > tamPista) {
    dzBot = -(tamPista - dzBot);
  }
  glPushMatrix();
  glTranslatef(Pontos.ponto[botPos + dzBot].x + dx, 0,
               Pontos.ponto[botPos + dzBot].z + pos -
                   (Pontos.ponto[botPos + dzBot].z + pos > 0 ? tamPista : 0));
  desenhaCarro(cor);
  glPopMatrix();
  if ((pos > (botPos + dzBot - 220) && pos < botPos + dzBot - 140) &&
      ((playerPos - 18 <= dx && playerPos >= dx) ||
       (playerPos + 18 >= dx && playerPos <= dx))) {
    colidiu = true;
    posQndoBateu = 0;
  }
  glutPostRedisplay();
}

void desenha() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_COLOR_MATERIAL);
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambiente);
  float difusao[] = {1.0, 1.0, 1.0, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, difusao);
  initScreen();

  glPushMatrix();
  desenhaPista();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(playerPos, 0, -180);
  glTranslatef(0, 0, -5);
  glTranslatef(0, 0, 5);
  glLightfv(GL_LIGHT0, GL_POSITION, posicao);
  desenhaCarro(vermelho);
  glPopMatrix();

  for (int i = 0; i < tamPista; i += 991) {
    glPushMatrix();
    if (contador == 0) {
      contador++;
      dxBot = 0;
      desenhaBot(corBot[contador], i, dxBot);
    } else if (contador == 1) {
      contador++;
      dxBot = 35;
      desenhaBot(corBot[contador], i, dxBot);
    } else if (contador == 2) {
      contador++;
      dxBot = 0;
      desenhaBot(corBot[contador], i, dxBot);
    } else if (contador == 3) {
      contador = 0;
      dxBot = -35;
      desenhaBot(corBot[contador], i, dxBot);
    }
    glPopMatrix();
  }

  while (pos >= tamPista)
    pos -= tamPista;
  while (pos < 0)
    pos += tamPista;
  while (botPos >= tamPista)
    botPos -= tamPista;
  while (botPos < 0)
    botPos += tamPista;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
  glLightfv(GL_LIGHT0, GL_POSITION, posicao);
  glFlush();
  glutSwapBuffers();
}

void gameKeyboard(unsigned char key, int x, int y) {
  int timerID = 0;
  switch (key) {
  case 27: // Tecla "ESC"
    exit(0);
    break;
  case 32: // Tecla "Espaço"
    ligado = ligado ? false : true;
    timerFunc(timerID);
    speed = 14;
    break;
  case 'w':
  case 'W':
    if (ligado && speed < 100) {
      pos += (0.12 * speed);
      botPos += 0.01 * speed;
      speed += 0.1;
    }
    break;
  case 's':
  case 'S':
    if (ligado && speed >= 0) {
      pos -= (0.12 * speed);
      botPos += 0.15 * speed;
      speed -= 0.1;
    }
    break;
  case 'a':
  case 'A':
    if (!tocandoEsquerda()) {
      playerPos -= 2;
    }
    break;
  case 'd':
  case 'D':
    if (!tocandoDireita()) {
      playerPos += 2;
    }
    break;

  default:
    break;
  }
  initScreen();
}

int main(int argc, char *argv[]) {
  srand(time(NULL));
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  sw = glutGet(GLUT_SCREEN_WIDTH);
  sh = glutGet(GLUT_SCREEN_HEIGHT);
  winPosX = (sw - sh) / 2;
  winPosY = 0;
  // iluminação
  posicao[0] = 0.0;
  posicao[1] = 22.0;
  posicao[2] = -40.0;
  posicao[3] = 1.0;
  glutInitWindowPosition(winPosX, winPosY);
  glutInitWindowSize(sh, sh);
  int c = 0;
  bool flagCor = false;
  x = 0;
  dx = 0;
  initArray(&Pontos, tamPista + 2);
  for (int i = 0; i < tamPista; i++) {
    Ponto_t ponto;
    ponto.x = 0;
    ponto.y = 0;
    ponto.z = -i;
    insertArray(&Pontos, ponto);
  }

  glutCreateWindow("Enduro");
  fitWindow(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  glutReshapeFunc(reshape);
  glutKeyboardFunc(gameKeyboard);
  glutTimerFunc(0, timerFunc, 1);
  glutDisplayFunc(desenha);
  glEnable(GL_DEPTH_TEST);
  initScreen();
  glutMainLoop();
}