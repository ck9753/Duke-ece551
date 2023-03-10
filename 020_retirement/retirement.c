#include <stdio.h>
#include <stdlib.h>

struct _retire_info {
  int months;
  double contribution;
  double rate_of_return;
};
typedef struct _retire_info retire_info;

void retirement(int startAge,         //in months
                double initial,       //initial savings in dollars
                retire_info working,  //info about working
                retire_info retired)  //info about being retired
{
  for (int i = startAge; i < startAge + working.months + retired.months; i++) {
    printf("Age %3d month %2d you have $%.2f\n", i / 12, i % 12, initial);

    if (i < startAge + working.months) {
      initial += initial * working.rate_of_return + working.contribution;
    }

    else if (i >= startAge + working.months) {
      initial += initial * retired.rate_of_return + retired.contribution;
    }
  }
}

int main(void) {
  retire_info working;
  working.months = 489;
  working.contribution = 1000;
  working.rate_of_return = 0.045 / 12;

  retire_info retired;
  retired.months = 384;
  retired.contribution = -4000;
  retired.rate_of_return = 0.01 / 12;

  retirement(327, 21345, working, retired);

  return EXIT_SUCCESS;
}
