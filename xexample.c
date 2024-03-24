#include "xdriver.h"
#include <stdio.h>
#include <stdlib.h>


/* X-Windows interface */


int maxx=600,maxy=300;
extern XColor x_tone_black[256],x_blue;



void linedemo()
{
  unsigned char cor=0;
  int i=0, key, mods,s=1;
    
  int xcoord[2]={150,450};
  int ycoord[2]={200,100};
  

  while(1)
    {
      clear_screen();       
      if(get_key(&key,&mods)) 
	{
	  switch(key)
	    {
	    case 27:return;
	      break;
	    case ' ':if(i==0) i=1;else i=0;
	      break;
	    case 'k':xcoord[i]++;
	      break;
	    case 'h':xcoord[i]--;
	      break;
	    case 'u':ycoord[i]--;
	      break;
	    case 'j':ycoord[i]++;
	      break;
	    }
	}

      drawcircle( xcoord[i], ycoord[i], 20 );  //sis
      line(xcoord[0], ycoord[0],xcoord[1], ycoord[1]); //minha

      // instruçoes
      if (cor==255) s=-1;else if(cor==0) s=1;
      draw_text(x_tone_black[cor+=s ],250,295,"KEYS: [esc]-Next demo [space]-Selec point      [j]down  ");
      draw_text(x_tone_black[cor],484,284,"[h]-left + [k]-right");
      draw_text(x_tone_black[cor],532,273,"[u]-up");
      draw_text(x_blue,0,10,"Line demo");
      //fim instruçoes     
      refresh_screen();   
    }
  
}

void circledemo()
{
  int i=0,s=1, key, mods;
  unsigned char cor=0;
  int xcoord[2]={300,100};
  int ycoord[2]={150,100};
  
  
  while(1)
    {
      clear_screen();       
      if(get_key(&key,&mods)) 
	{
	  switch(key)
	    {
	    case 27:return;
	      break;
	    case ' ':if(i==0) i=1;else i=0;
	      break;
	    case 'k':xcoord[i]++;
	      break;
	    case 'h':xcoord[i]--;
	      break;
	    case 'u':ycoord[0]--;
	      break;
	    case 'j':ycoord[0]++;
	      break;
	    }
	}
    
      
      if (i==0)
	drawcircle( xcoord[i], ycoord[0], 20 );  //sis
      else drawcircle( xcoord[i]+xcoord[0], ycoord[0], 20 ); //sis
      
      circle(xcoord[0],ycoord[0],xcoord[1]); //minha função 

      // instruçoes
      if (cor==255) s=-1;else if(cor==0) s=1;
      draw_text(x_tone_black[cor+=s ],250,295,"KEYS: [esc]-Next demo [space]-Selec point      [j]down  ");
      draw_text(x_tone_black[cor],484,284,"[h]-left + [k]-right");
      draw_text(x_tone_black[cor],532,273,"[u]-up");
      draw_text(x_blue,0,10,"Circle demo");
      //fim instruçoes          
      refresh_screen();   
    }
}


int polydemo()
{
  int i=1,s=1, key, mods;
  int px,py,qx,qy;
  
  int xpoly[6]={150,200,300,350,300,200};
  int ypoly[6]={150,60,60,150,240,240};

  unsigned char cor=0;  
  //
  
  while(1)
	{
	  clear_screen();       
	  
	  if(get_key(&key,&mods)) 
	    {
	      switch(key)
		{
		case 27:return;
		  break;
		case ' ':i++;if(i>5) i=0;
		  break;
		case 'k':xpoly[i]++;
		  break;
		case 'h':xpoly[i]--;
		  break;
		case 'u':ypoly[i]--;
		  break;
		case 'j':ypoly[i]++;
		  break;
		}
	    }
	  
	  drawcircle( xpoly[i], ypoly[i], 20 ); //função do sisteme + rapid
	  
	  
	 drawfilledpoligon(xpoly,ypoly,6); // enchimento de um polígono com coord. inteiras
      // instruçoes
      if (cor==255) s=-1;else if(cor==0) s=1;
      draw_text(x_tone_black[cor+=s ],250,295,"KEYS: [esc]-Next demo [space]-Selec point      [j]down  ");
      draw_text(x_tone_black[cor],484,284,"[h]-left + [k]-right");
      draw_text(x_tone_black[cor],532,273,"[u]-up");
      draw_text(x_blue,0,10,"Poligon demo");
      //fim instruçoes
	  
	  refresh_screen();   
	}
  
}

void clipdemo()
{
  int i=0,j,s=1, key, mods;
  unsigned char cor=0;
  Point caixa[2];
  Point linhas[6];

  linhas[0].x=20;linhas[0].y=150;          // 0-1 uma linha
  linhas[1].x=420,linhas[1].y=150;

  linhas[2].x=20;linhas[2].y=40;          // 2-3 uma linha
  linhas[3].x=500,linhas[3].y=250;

  linhas[4].x=480;linhas[4].y=60;          // 4-5 uma linha
  linhas[5].x=400,linhas[5].y=250;
 
  int xcoord[6]={150,200,300,350,300,200};   // 3 linhas
  int ycoord[6]={150,60,60,150,240,240};
  
  caixa[0].x=50;caixa[0].y=50;              //caixa para clip
  caixa[1].x=550;caixa[1].y=250;


 while(1)
	{
	  clear_screen();       
	  
	  if(get_key(&key,&mods)) 
	    {
	      switch(key)
		{
		case 27:return;
		  break;
		case ' ':i++;if(i>1) i=0;
		  break;
		case 'k':caixa[i].x+=2;
		  break;
		case 'h':caixa[i].x-=2;
		  break;
		case 'u':caixa[i].y-=2;
		  break;
		case 'j':caixa[i].y+=2;
		  break;
		}
	    }

	  drawcircle( caixa[i].x, caixa[i].y, 20 ); //função do sisteme + rapid
	  for(j=0;j<6;j+=2)
	    clipLine(caixa[0],caixa[1],linhas[j],linhas[j+1]);
	  
	  //drawLine(linhas[0],linhas[1]);

	  drawRectangle(caixa[0],caixa[1]);
      // instruçoes
      if (cor==255) s=-1;else if(cor==0) s=1;
      draw_text(x_tone_black[cor+=s ],250,295,"KEYS: [esc]-Next demo [space]-Selec point      [j]down  ");
      draw_text(x_tone_black[cor],484,284,"[h]-left + [k]-right");
      draw_text(x_tone_black[cor],532,273,"[u]-up");
      draw_text(x_blue,0,10,"ClipLine demo");
      //fim instruçoes

	  
	  refresh_screen();   
	}


}


void smoothlinedemo()
{
  int i=0, key, mods,s=1;
  unsigned char cor=0;
  Point pontos[2];

  pontos[0].x=150;pontos[0].y=100;     // dois pontos
  pontos[1].x=450;pontos[1].y=200;

  while(1)
    {
      clear_screen();       
      if(get_key(&key,&mods)) 
	{
	  switch(key)
	    {
	    case 27:return;
	      break;
	    case ' ':if(i==0) i=1;else i=0;
	      break;
	    case 'k':pontos[i].x++;
	      break;
	    case 'h':pontos[i].x--;
	      break;
	    case 'u':pontos[i].y--;
	      break;
	    case 'j':pontos[i].y++;
	      break;
	    }
	}

      drawCircle( pontos[i], 20 ); 
      drawSmoothLine(pontos[0],pontos[1]);
      // instruçoes
      if (cor==255) s=-1;else if(cor==0) s=1;
      draw_text(x_tone_black[cor+=s ],250,295,"KEYS: [esc]-Quit      [space]-Selec point      [j]down  ");
      draw_text(x_tone_black[cor],484,284,"[h]-left + [k]-right");
      draw_text(x_tone_black[cor],532,273,"[u]-up");
      draw_text(x_blue,0,10,"Smoothline demo");
      //fim instruçoes


      refresh_screen();   
    }
  
}

int main( int argc, char *argv[] )
{
  init_driver("C.G. trab1",maxy,maxx);

  linedemo(); 
  circledemo();
  polydemo();
  clipdemo();
  smoothlinedemo();
  return 0;
  
}


