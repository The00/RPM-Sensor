#include "rpm.h"
#include "Arduino.h"


/// global variable
static volatile unsigned long Dtime;
static volatile unsigned long Dtime1;
static volatile byte i;
static volatile byte it_done;
static byte it;


/// callback function of the interrupt

void rpm_isr()
{ Dtime=Dtime1;
  Dtime1 =micros();
  if(Dtime > 0) it_done=1;
}


/// setup global variable and the ISR.

void rpm_init(int num_it)
{
    Dtime=0;
    Dtime1=0;
    i =0;
    it_done =0;
	it = num_it;
    attachInterrupt(num_it,rpm_isr,RISING);
}


/// return the mean of the given array

float get_mean(int sizeTab,int data[])
{
    byte i_local;
    unsigned long result=0;

    for(i_local=0;i_local<sizeTab;i_local++) result +=data[i_local];

    return (float(result)/sizeTab);
}


/// return the standard deviation (unit : µs)  of the given array

float get_sd(int sizeTab,int data[],float mean)
{
    byte i_local;
    float result=0;

    for(i_local=0;i_local<sizeTab;i_local++) result +=sq(data[i_local]-mean);

    return sqrt(result/sizeTab);
}


void switch_extremum(int sizeTab,int data[],float mean)
{
    byte i_local=0;
    byte i_extr=0;
    float transit=0;
    float transit2=0;

    transit=abs(data[i_local]-mean);
    for(i_local=1;i_local<sizeTab;i_local++)
    {
       transit2=abs(data[i_local]-mean);
       if(transit2>transit)
       {
           i_extr=i_local;
           transit=transit2;
       }
    }

    data[i_extr]=data[sizeTab-1];
}


/// recursive function that take off the values too far from the mean until the standard deviance is low enough

void analyse_data(int sizeTab, int data[], rpm_data *result)
{
    static byte nb_call =0;

    result->rpm = get_mean(sizeTab,data);
    result->sd = get_sd(sizeTab,data,result->rpm);

    if((result->sd) > MAX_ECARTYPE) // si l'ecart type est encore trop grand
    {
        if(sizeTab > ((sizeTab+nb_call)/2)) // si on a éliminé moins de la moitié des valeurs on continue a enlever des valeurs
        {   nb_call++;
            switch_extremum(sizeTab,data,result->rpm);
            analyse_data(sizeTab-1,data,result);
        }
        else // la série de valeurs est inexploitable
        {
            result->rpm=-1;
            result->nbr_sample_r=sizeTab;
        }
    }
    else result->nbr_sample_r=sizeTab;

    nb_call=0;
}



rpm_data get_rpm_data(int nbr_sample,int timeout, int divider)
{

  unsigned long init_time=millis();
  unsigned long time=init_time;
  int sample[MAX_SAMPLE]={0};
  int period;
  rpm_data result;

  i=0;
  it_done=0;

    if(nbr_sample > MAX_SAMPLE) nbr_sample = MAX_SAMPLE;
    if(timeout > MAX_TIMEOUT) timeout = MAX_TIMEOUT;
   // init_rpm();

   while (i<nbr_sample && ((time - init_time)<timeout))
  {
    time=millis();
    if(it_done)
    {
      period=Dtime1-Dtime;
      if(period>20)
        {
        sample[i]=period;
        i++;
        }
      it_done=0;
     }
  }

    analyse_data(i,sample,&result);

    if(result.rpm >0) result.rpm =  60/(result.rpm*pow(10,-6));  // conversion periode -> tr/mn
    if(divider > 0) result.rpm /= divider;

    result.time = millis()-init_time;

    return result;
}


float get_rpm(int nbr_sample,int timeout, int divider)
{
    return (get_rpm_data(nbr_sample,timeout,divider).rpm);
}


void rpm_stop_isr()
{
	detachInterrupt(it);
}