#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

byte spidata[16];
void spiTransfer(int addr, byte opcode, byte data);
int SPI_MOSI;
int SPI_CLK;
int SPI_CS;
int maxDevices;
        
void spiTransfer(int addr, volatile byte opcode, volatile byte data) {
  int offset=addr*2;
  int maxbytes=maxDevices*2;
  for(int i=0;i<maxbytes;i++)
    spidata[i]=(byte)0;
  spidata[offset+1]=opcode;
  spidata[offset]=data;
  digitalWrite(SPI_CS,LOW);
  for(int i=maxbytes;i>0;i--)
    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
  digitalWrite(SPI_CS,HIGH);
}

void setScanLimit(int addr, int limit) {
  if(addr<0 || addr>=maxDevices)
    return;
  if(limit>=0 && limit<8)
    spiTransfer(addr, OP_SCANLIMIT,limit);
}

void shutdown(int addr, bool b) {
  if(addr<0 || addr>=maxDevices)
    return;
  if(b)
    spiTransfer(addr, OP_SHUTDOWN,0);
  else
    spiTransfer(addr, OP_SHUTDOWN,1);
}


void setIntensity(int addr, int intensity) {
  if(addr<0 || addr>=maxDevices)
    return;
  if(intensity>=0 && intensity<16)  
    spiTransfer(addr, OP_INTENSITY,intensity);
}

void begin(int dataPin, int clkPin, int csPin, int numDevices){
  SPI_MOSI=dataPin;
  SPI_CLK=clkPin;
  SPI_CS=csPin;
  if(numDevices<=0 || numDevices>8 )
    numDevices=8;
  maxDevices=numDevices;
  pinMode(SPI_MOSI,OUTPUT);
  pinMode(SPI_CLK,OUTPUT);
  pinMode(SPI_CS,OUTPUT);
  digitalWrite(SPI_CS,HIGH);
  SPI_MOSI=dataPin;
  for(int i=0;i<maxDevices;i++) {
    spiTransfer(i,OP_DISPLAYTEST,0);
    setScanLimit(i,7);
    spiTransfer(i,OP_DECODEMODE,0);
    shutdown(i,true);
  }
  for(int address=0; address<maxDevices; address++) {
    shutdown(address,false);
  }
}

void setRow(int addr, int row, byte value) {
  if(addr<0 || addr>=maxDevices)
    return;
  if(row<0 || row>7)
    return;
  spiTransfer(addr, row+1,value);
}

unsigned char frame[32][8];

const unsigned char Block[8][4][4][4]={
0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,

0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,

0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,

0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,


0,0,0,0,
1,1,1,1,
0,0,0,0,
0,0,0,0,

0,0,1,0,
0,0,1,0,
0,0,1,0,
0,0,1,0,

0,0,0,0,
1,1,1,1,
0,0,0,0,
0,0,0,0,

0,0,1,0,
0,0,1,0,
0,0,1,0,
0,0,1,0,


0,0,0,0,
2,2,2,0,
2,0,0,0,
0,0,0,0,

0,2,0,0,
0,2,0,0,
0,2,2,0,
0,0,0,0,

0,0,0,0,
0,0,2,0,
2,2,2,0,
0,0,0,0,

2,2,0,0,
0,2,0,0,
0,2,0,0,
0,0,0,0,


0,0,0,0,
6,6,6,0,
0,0,6,0,
0,0,0,0,

0,6,6,0,
0,6,0,0,
0,6,0,0,
0,0,0,0,

0,0,0,0,
6,0,0,0,
6,6,6,0,
0,0,0,0,

0,6,0,0,
0,6,0,0,
6,6,0,0,
0,0,0,0,


0,0,0,0,
5,5,5,0,
0,5,0,0,
0,0,0,0,

0,5,0,0,
0,5,5,0,
0,5,0,0,
0,0,0,0,

0,0,0,0,
0,5,0,0,
5,5,5,0,
0,0,0,0,

0,5,0,0,
5,5,0,0,
0,5,0,0,
0,0,0,0,


0,0,0,0,
4,4,0,0,
0,4,4,0,
0,0,0,0,

0,0,4,0,
0,4,4,0,
0,4,0,0,
0,0,0,0,

0,0,0,0,
4,4,0,0,
0,4,4,0,
0,0,0,0,

0,0,4,0,
0,4,4,0,
0,4,0,0,
0,0,0,0,


0,0,0,0,
0,7,7,0,
7,7,0,0,
0,0,0,0,

7,0,0,0,
7,7,0,0,
0,7,0,0,
0,0,0,0,

0,0,0,0,
0,7,7,0,
7,7,0,0,
0,0,0,0,

7,0,0,0,
7,7,0,0,
0,7,0,0,
0,0,0,0,


0,0,0,0,
0,3,3,0,
0,3,3,0,
0,0,0,0,

0,0,0,0,
0,3,3,0,
0,3,3,0,
0,0,0,0,

0,0,0,0,
0,3,3,0,
0,3,3,0,
0,0,0,0,

0,0,0,0,
0,3,3,0,
0,3,3,0,
0,0,0,0,
};

void clear(){
  for(int y=0;y<32;y++){
    for(int x=0;x<8;x++){
      frame[y][x]=0;
    }
  }
}

void present(){
  for(int i=0;i<4;i++){
    int v[8]={0,0,0,0,0,0,0,0};
    for(int y=0;y<8;y++){
      for(int x=0;x<8;x++){
        v[x]|=frame[i*8+y][7-x]<<(7-y);
      }
    }
    for(int x=0;x<8;x++){
      setRow(i,x,v[x]);
    }
  }
}

template<int width,int height>
class BlockStage
{
  uint8_t buffer[height+6][width+4];
  uint8_t *center[height+6];
public:
  BlockStage();
  void Render();
  float Gravity;
  float Seti;
  void Fall(float time);
  float jikan;

  int x;
  float y;
  float stop;
  int type;
  int r;
  int irs;
  int next;
  int prev;
  float dl;
  float delay;
  bool HitCheck(int Type,int R,int X,int Y);

  void Left();
  void Right();
  void Down();
  void Hidown();
  void Ror();
  void Rol();
  void Refresh();
  int combo;

  void Start();
  float over;
  void Set();

  int level;
  int score;
  int line;
};

BlockStage<8,31> bs;

float left;
float right;
float down;
float ror;
float rol;
float roll;

void setup() {
  begin(12, 10, 11, 4); // dataPin, clkPin, csPin, numero de matrices de 8x8
  for(int i=0;i<4;i++){
    setIntensity(i,1);
  }
  left=0;
  right=0;
  down=0;
  ror=0;
  rol=0;
  roll=0;

  bs.Start();
  clear();
  present();
  Serial.begin(9600);
}

void loop() {
  if(bs.over>10000){
    bs.Start();
  }
  
  int JoyX=0;
  int JoyY=0;
  int r_button=0;
  int l_button=0;
  if (Serial.available() > 0) {
    switch(Serial.read()){
      case 'w':l_button=1;break;
      case 'r':r_button=1;break;
      case 's':JoyX=-1000;break;
      case 'd':JoyY=-1000;break;
      case 'f':JoyX=+1000;break;
    }
  }
        
  int Time=50;
  
  if(JoyX<-500) {
    if(left) {
      left+=Time;
      while(left>100) {
        bs.Left();
        left-=15;
      }
    }
    else {
      bs.Left();
      left=Time;
    }
  }
  else
    left=0;
  if(JoyX>500) {
    if(right) {
      right+=Time;
      while(right>100) {
        bs.Right();
        right-=15;
      }
    }
    else {
      bs.Right();
      right=Time;
    }
  }
  else
    right=0;
  if(JoyY>500) {
    if(down) {
      down+=Time;
      while(down>150) {
        bs.Down();
        down-=15;
      }
    }
    else {
      bs.Down();
      down=Time;
    }
  }
  else
    down=0;
  if(JoyY<-500)
    bs.Hidown();
  if(r_button) {
    if(ror)
      ror+=Time;
    else {
      bs.Ror();
      ror=Time;
    }
    bs.irs=3;
  }
  else
    ror=0;
    
  if(l_button) {
    if(rol)
      rol+=Time;
    else {
      bs.Rol();
      rol=Time;
    }
    bs.irs=1;
  }
  else
    rol=0;

  bs.Fall(Time);
  bs.Render();
  present();
}

template<int width,int height>
BlockStage<width,height>::BlockStage()
{
  int x=width;
  int y=height;
  for(int i=0;i<y+6;i++)
  {
    center[i]=buffer[i]+2;
    for(int j=0;j<x+4;j++)
    {
      if(i<1||j<2||i>=2+y||j>=2+x)
        buffer[i][j]=8;
      else
        buffer[i][j]=0;
    }
  }

  type=0;
  stop=0;
  irs=0;
  dl=0;
  over=0;
  next=0;
  level=0;
  score=0;
  jikan=0;
  line=0;
}

template<int width,int height>
void BlockStage<width,height>::Start()
{
  for(int i=-1;i<height;i++)
    memset(center[2+i],0,width);
  type=0;
  stop=0;
  irs=0;
  dl=0;
  over=0;
  level=0;
  delay=0;
  score=0;
  jikan=0;
  line=0;
  combo=0;
  {
    Gravity=2/2000.0f;
    Seti=500;
  }
  
  next=random(7)+1;
}

template<int width,int height>
void BlockStage<width,height>::Refresh()
{
  if(stop)
  {
    if(!HitCheck(type,r,x,(int)y+1))
      stop=0;
  }
}

template<int width,int height>
void BlockStage<width,height>::Right()
{
  if(delay||!type)
    return;
  if(!HitCheck(type,r,x+1,(int)y))
  {
    if(stop)
    {
      x++;
      Refresh();
    }
    else
    {
      if(!HitCheck(type,r,x+1,(int)y+1))
      {
        x++;
        Refresh();
      }
      else if(y-floorf(y)>0&&y-floorf(y)<0.1f)
      {
        x++;
        Refresh();
      }
    }
  }
}

template<int width,int height>
void BlockStage<width,height>::Left()
{
  if(delay||!type)
    return;
  if(!HitCheck(type,r,x-1,(int)y))
  {
    if(stop)
    {
      x--;
      Refresh();
    }
    else
    {
      if(!HitCheck(type,r,x-1,(int)y+1))
      {
        x--;
        Refresh();
      }
      else if(y-floorf(y)>0&&y-floorf(y)<0.1f)
      {
        x--;
        Refresh();
      }
    }
  }
}

template<int width,int height>
void BlockStage<width,height>::Down()
{
  if(delay||!type)
    return;
  if(stop)
  {
    stop=10000000;
  }
  else
  {
    dl=0.5f;
  }
}

template<int width,int height>
void BlockStage<width,height>::Hidown()
{
  if(stop||!type||delay)
    return;
  for(;;)
  {
    y=floorf(y);
    if(HitCheck(type,r,x,(int)y+1))
    {
      stop=1;
      y=floorf(y);
      return;
    }
    y++;
  }
}

template<int width,int height>
void BlockStage<width,height>::Ror()
{
  if(delay||!type)
    return;
  if(!HitCheck(type,r-1,x,(int)y))
  {
    if(stop)
    {
      r--;
      Refresh();
      return;
    }
    else if(!HitCheck(type,r-1,x,(int)y+1))
    {
      r--;
      Refresh();
      return;
    }
  }
  else if(!HitCheck(type,r-1,x+1,(int)y))
  {
    if(stop)
    {
      r--;
      x++;
      Refresh();
      return;
    }
    else if(!HitCheck(type,r-1,x+1,(int)y+1))
    {
      r--;
      x++;
      Refresh();
      return;
    }
  }
  else if(!HitCheck(type,r-1,x-1,(int)y))
  {
    if(stop)
    {
      r--;
      x--;
      Refresh();
      return;
    }
    else if(!HitCheck(type,r-1,x-1,(int)y+1))
    {
      r--;
      x--;
      Refresh();
      return;
    }
  }
}

template<int width,int height>
void BlockStage<width,height>::Rol()
{
  if(delay||!type)
    return;
  if(!HitCheck(type,r+1,x,(int)y))
  {
    if(stop)
    {
      r++;
      Refresh();
      return;
    }
    else if(!HitCheck(type,r+1,x,(int)y+1))
    {
      r++;
      Refresh();
      return;
    }
  }
  else if(!HitCheck(type,r+1,x+1,(int)y))
  {
    if(stop)
    {
      r++;
      x++;
      Refresh();
      return;
    }
    else if(!HitCheck(type,r+1,x+1,(int)y+1))
    {
      r++;
      x++;
      Refresh();
      return;
    }
  }
  else if(!HitCheck(type,r+1,x-1,(int)y))
  {
    if(stop)
    {
      r++;
      x--;
      Refresh();
      return;
    }
    else if(!HitCheck(type,r+1,x-1,(int)y+1))
    {
      r++;
      x--;
      Refresh();
      return;
    }
  }
}

template<int width,int height>
void BlockStage<width,height>::Set()
{
  int l,s=0;;
  for(int i=0;i<height;i++)
  {
    l=0;
    for(int j=0;j<width;j++)
      if(center[2+i][j])
        l++;
    if(l==width)
    {
      for(int j=i;j>0;j--)
      {
        memcpy(center[2+j],center[2+j-1],width);
      }
      memset(center[2],0,width);
      s++;
    }
  }
  if(s)
  {
    combo++;
    switch(s)
    {
    case 1:
      score+=1*level*combo;
      break;
    case 2:
      score+=3*level*combo;
      break;
    case 3:
      score+=6*level*combo;
      break;
    case 4:
      score+=10*level*combo;
      break;
    }
    combo+=s;
    line+=s;
    level+=s;
    if(level%100-s<=0)
    {
      score+=level/100*1000;
    }
    Gravity+=s/20000.0f;
  }
  else
    combo=0;
}

template<int width,int height>
void BlockStage<width,height>::Fall(float time)
{
  if(over)
  {
    over+=time;
    return;
  }
  if(r==-1)
    r=3;
  if(r==4)
    r=0;
  jikan+=time;
  if(!type)
  {
    type=next;
    r=irs;
    irs=0;
    stop=0;
    next=rand()%7+1;
    if(prev==next)
      next=rand()%7+1;
    x=2;
    y=-0.999f;
    if(HitCheck(type,r,x,(int)floorf(y)))
    {
      for(int i=0;i<4;i++)
      {
        for(int j=0;j<4;j++)
        {
          if(Block[type][r][i][j])
            center[2+(int)floorf(y)+i][x+j]=Block[type][r][i][j];
        }
      }
      over=1;
      type=0;
      next=0;
      return;
    }
    if(level%100!=99)
      level++;
    Gravity+=1/20000.0f;
    if(level>100)
      Seti-=0.05f;
    if(level>200)
      Seti-=0.2f;
  }
  if(delay)
  {
    delay+=time;
    if(delay>100)
    {
      for(int i=0;i<4;i++)
      {
        for(int j=0;j<4;j++)
        {
          if(Block[type][r][i][j])
            center[2+(int)floorf(y)+i][x+j]=Block[type][r][i][j];
        }
      }
      Set();
      prev=type;
      type=0;
      delay=0;
    }
    return;
  }
  if(!stop)
  {
    float add=Gravity*time+dl;
    do
    {
      y+=add>1?1:add;
      add-=1;
      if(HitCheck(type,r,x,(int)floorf(y)+1))
      {
        stop=1;
        y=floorf(y);
        break;
      }
    }
    while(add>0);
  }
  else
  {
    stop+=time;
    if(stop>Seti)
    {
      delay=1;
      stop=0;
    }
  }
  dl=0;
  irs=0;
}

template<int width,int height>
bool BlockStage<width,height>::HitCheck(int Type,int R,int X,int Y)
{
  if(R==-1)
    R=3;
  if(R==4)
    R=0;
  if(Y>=height)
    return true;
  for(int i=0;i<4;i++)
  {
    for(int j=0;j<4;j++)
    {
      if(center[2+i+Y][j+X]&&Block[Type][R][i][j])
        return true;
    }
  }
  return false;
}

template<int width,int height>
void BlockStage<width,height>::Render(){
  for(int y=0;y<height;y++){
    for(int x=0;x<width;x++){
      frame[1+y][x]=center[y+2][x]?1:0;
    }
  }
  if(type){
    for(int i=0;i<4;i++){
      for(int j=0;j<4;j++) {
        if(Block[type][r][i][j])  {
          frame[1+(int)y+i][x+j]=1;
        }
      }
    }
  }
  for(int i=0;i<8;i++){
    frame[0][7-i]=(line>>i)%2;
  }
}

