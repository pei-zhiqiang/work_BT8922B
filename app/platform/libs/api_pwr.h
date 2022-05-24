#ifndef _API_PWR_H_
#define _API_PWR_H_

void pmu_init(u8 buck_en);
u16 pmu_get_vbg(void);
u16 pmu_get_vddio(void);
void set_buck_mode(u8 buck_en);
#endif // _API_PWR_H_

