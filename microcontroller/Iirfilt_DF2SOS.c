
#include "Iirfilt_DF2SOS.h"

/* Main
    Iir_2order iir1;
    Iir_2order iir2;
    init_iir(&iir1, 0.991153597831726, 1, -1.432918429374695, 0.982220232486725, 1, -1.458052396774292, 1);
    init_iir(&iir2, 0.991153597831726, 1, -1.457277178764343, 0.982550680637360, 1, -1.458052396774292, 1);
    sal_iir1 = calc_iir(i[0], &iir1);
    adc_acum + = calc_iir(sal_iir1, &iir2);
 
 */


/*INICIAN FUNCIONES*/
  int calc_iir(int x_int, Iir_2order *ap_iir){
  float x=0;
  float wn=0;
  float y=0;

  x = ((float)x_int)/(float)4096;

  wn = (1/ap_iir->a1)*((ap_iir->k1)*x - (ap_iir->a2)*(ap_iir->wn1) - (ap_iir->a3)*(ap_iir->wn2));
  y = (ap_iir->b1)*wn + (ap_iir->b2)*(ap_iir->wn1) + (ap_iir->b3)*(ap_iir->wn2);

  ap_iir->wn2 = ap_iir->wn1;
  ap_iir->wn1 = wn;

  y = y*(float)4096;
  
  return (int) y;
  
  }

  void init_iir(Iir_2order*ap_iir, float k1, float a1, float a2, float a3, float b1, float b2, float b3)
{
  ap_iir->k1 = k1;
  ap_iir->a1 = a1;
  ap_iir->a2 = a2;
  ap_iir->a3 = a3;
  ap_iir->b1 = b1;
  ap_iir->b2 = b2;
  ap_iir->b3 = b3;
}

