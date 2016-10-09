#ifndef RPM_H_INCLUDED
#define RPM_H_INCLUDED

#define MAX_ECARTYPE 25 // maximum standard deviation in µs
#define MAX_SAMPLE 60   // maximum number of sample for a measure
#define MAX_TIMEOUT 200 // maximun sample time in ms

struct rpm_data{

    float rpm; // vitesse mesurée
    float sd; // permet de voir si la mesure est valable
    int time;  // temps d'execution de la mesure en ms
    int nbr_sample_r;

} ;
typedef struct rpm_data rpm_data;

void rpm_isr();
void rpm_init(int num_it);
rpm_data get_rpm_data(int nbr_sample,int timeout, int divider =1);
float get_rpm(int nbr_sample,int timeout,int divider =1);
void rpm_stop_isr();

#endif // RPM_H_INCLUDED
