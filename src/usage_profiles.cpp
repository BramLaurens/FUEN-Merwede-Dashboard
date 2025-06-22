#include "usage_profiles.h"

const long usage_profile_base[24] = {
    // Usage profile for a typical day. no PV. With HP, With EV.

    600000,   // 00:00 - Late night, low usage
    500000,   // 01:00
    450000,   // 02:00
    450000,   // 03:00
    550000,   // 04:00 - Slight rise before morning
    3000000,  // 05:00 - Morning ramp-up starts
    6000000,  // 06:00
    9000000,  // 07:00 - Morning peak ~9MW
    7000000,  // 08:00 - Slight dip after peak
    4000000,  // 09:00 - Mid-morning decline
    3000000,  // 10:00 - Steady daytime usage
    2500000,  // 11:00
    2700000,  // 12:00
    2600000,  // 13:00
    2800000,  // 14:00
    3200000,  // 15:00 - Start of evening ramp
    4000000,  // 16:00
    7000000,  // 17:00
    8500000,  // 18:00 - Evening peak ~9MW
    9000000,  // 19:00 - Max daily usage
    7000000,  // 20:00 - Start of decline
    4000000,  // 21:00
    3000000,  // 22:00
    1500000   // 23:00 - Late evening
};

const long usage_profile_with_PV[24] = {
    // Usage profile for a typical day with PV generation. With HP, With EV.

    1200000,   // 00:00 - night, low usage
    1000000,   // 01:00
    950000,    // 02:00
    900000,    // 03:00
    1100000,   // 04:00 - early morning rise
    3000000,   // 05:00 - morning ramp-up
    5500000,   // 06:00
    8800000,   // 07:00 - morning peak ~9MW
    7500000,   // 08:00 - still high
    4000000,   // 09:00 - tapering
    1000000,   // 10:00 - PV starts producing
    -1000000,  // 11:00 - net export
    -2500000,  // 12:00 - peak solar generation
    -2000000,  // 13:00
    -1500000,  // 14:00
    -500000,   // 15:00 - PV fading
    1000000,   // 16:00 - back to net usage
    4500000,   // 17:00 - evening ramp
    8500000,   // 18:00 - evening peak
    9000000,   // 19:00 - max daily usage
    7000000,   // 20:00 - still high
    4000000,   // 21:00
    2000000,   // 22:00
    1400000    // 23:00 - night again
};

const long usage_profile_nohp_base[24]{
    // Usage profile for a typical day. no HP, With EV.

    600000,   // 00:00 - Late night, low usage
    500000,   // 01:00
    450000,   // 02:00
    450000,   // 03:00
    550000,   // 04:00 - Slight rise before morning
    2500000,  // 05:00 - Morning ramp-up starts
    5000000,  // 06:00
    7000000,  // 07:00 - Morning peak ~7MW
    5500000,  // 08:00 - Slight dip after peak
    3500000,  // 09:00 - Mid-morning decline
    2500000,  // 10:00 - Steady daytime usage
    2200000,  // 11:00
    2300000,  // 12:00
    2200000,  // 13:00
    2400000,  // 14:00
    2700000,  // 15:00 - Start of evening ramp
    3200000,  // 16:00
    5500000,  // 17:00
    6700000,  // 18:00 - Evening peak ~7MW
    7000000,  // 19:00 - Max daily usage
    5500000,  // 20:00 - Start of decline
    3500000,  // 21:00
    2500000,  // 22:00
    1500000   // 23:00 - Late evening
};

const long usage_profile_noev_base[24]{
// Usage profile for a typical day. with HP, no EV.

    600000,   // 00:00 - Late night, low usage
    500000,   // 01:00
    450000,   // 02:00
    450000,   // 03:00
    550000,   // 04:00 - Slight rise before morning
    2500000,  // 05:00 - Morning ramp-up starts
    5000000,  // 06:00
    7000000,  // 07:00 - Morning peak ~7MW
    5500000,  // 08:00 - Slight dip after peak
    3500000,  // 09:00 - Mid-morning decline
    2500000,  // 10:00 - Steady daytime usage
    2200000,  // 11:00
    2300000,  // 12:00
    2200000,  // 13:00
    2400000,  // 14:00
    2700000,  // 15:00 - Start of evening ramp
    3200000,  // 16:00
    5500000,  // 17:00
    6700000,  // 18:00 - Evening peak ~7MW
    7000000,  // 19:00 - Max daily usage
    5500000,  // 20:00 - Start of decline
    3500000,  // 21:00
    2500000,  // 22:00
    1500000   // 23:00 - Late evening
};

const long usage_profile_nohp_with_PV[24]{
    // Usage profile for a typical day with PV generation. No HP. With EV.
    1200000,   // 00:00 - night, low usage
    1000000,   // 01:00
    950000,    // 02:00
    900000,    // 03:00
    1100000,   // 04:00 - early morning rise

    2200000,   // 05:00 - morning ramp-up
    4000000,   // 06:00
    7000000,   // 07:00 - morning peak ~7MW
    6000000,   // 08:00 - still high
    3500000,   // 09:00 - tapering

    900000,    // 10:00 - PV starts producing
    -1500000,  // 11:00 - stronger net export
    -3500000,  // 12:00 - peak solar generation (more negative)
    -3000000,  // 13:00
    -2500000,  // 14:00
    -1000000,  // 15:00 - PV fading

    900000,    // 16:00 - back to net usage
    3000000,   // 17:00 - evening ramp
    6500000,   // 18:00 - evening peak
    7000000,   // 19:00 - max daily usage
    5500000,   // 20:00 - still high
    3500000,   // 21:00
    2000000,   // 22:00
    1400000    // 23:00 - night again
};

const long usage_profile_noev_with_PV[24]{
    // Usage profile for a typical day with PV generation. With HP. No EV.
    1200000,   // 00:00 - night, low usage
    1000000,   // 01:00
    950000,    // 02:00
    900000,    // 03:00
    1100000,   // 04:00 - early morning rise

    2200000,   // 05:00 - morning ramp-up
    4000000,   // 06:00
    7000000,   // 07:00 - morning peak ~7MW
    6000000,   // 08:00 - still high
    3500000,   // 09:00 - tapering

    900000,    // 10:00 - PV starts producing
    -1500000,  // 11:00 - stronger net export
    -3500000,  // 12:00 - peak solar generation (more negative)
    -3000000,  // 13:00
    -2500000,  // 14:00
    -1000000,  // 15:00 - PV fading

    900000,    // 16:00 - back to net usage
    3000000,   // 17:00 - evening ramp
    6500000,   // 18:00 - evening peak
    7000000,   // 19:00 - max daily usage
    5500000,   // 20:00 - still high
    3500000,   // 21:00
    2000000,   // 22:00
    1400000    // 23:00 - night again
};

const long usage_profile_nohp_noev_base[24]{
    500000,   // 00:00 - Late night, very low usage
    450000,   // 01:00
    400000,   // 02:00
    400000,   // 03:00
    450000,   // 04:00 - Slight rise before morning
    1500000,  // 05:00 - Morning ramp-up
    2500000,  // 06:00
    4500000,  // 07:00 - Morning peak
    4000000,  // 08:00 - Still high
    2500000,  // 09:00
    2000000,  // 10:00 - Daytime plateau
    1800000,  // 11:00
    1850000,  // 12:00
    1800000,  // 13:00
    1900000,  // 14:00
    2100000,  // 15:00 - Gradual climb
    2700000,  // 16:00
    4000000,  // 17:00
    4900000,  // 18:00 - Evening peak
    5000000,  // 19:00 - Max usage of the day
    4000000,  // 20:00 - Declining
    2700000,  // 21:00
    1800000,  // 22:00
    1200000   // 23:00 - Night again
};

const long usage_profile_nohp_noev_with_PV[24]{
    1000000,   // 00:00 - night, low usage
    900000,    // 01:00
    850000,    // 02:00
    800000,    // 03:00
    900000,    // 04:00
    1800000,   // 05:00 - morning ramp-up
    3000000,   // 06:00
    5000000,   // 07:00 - morning peak
    4200000,   // 08:00
    2500000,   // 09:00
    500000,    // 10:00 - solar kicking in
    -2000000,  // 11:00 - net PV export
    -4000000,  // 12:00 - PV peak production
    -3500000,  // 13:00
    -3000000,  // 14:00
    -1500000,  // 15:00
    500000,    // 16:00 - net consumption resumes
    2800000,   // 17:00 - evening ramp-up
    4700000,   // 18:00 - evening peak
    5000000,   // 19:00 - max daily usage
    4000000,   // 20:00
    2800000,   // 21:00
    1800000,   // 22:00
    1200000    // 23:00
};

const long usage_profile_base_BATT[24]{
    800000,   // 00:00 - night
    700000,   // 01:00
    650000,   // 02:00
    650000,   // 03:00
    750000,   // 04:00

    2500000,  // 05:00 - ramp-up
    4000000,  // 06:00
    5000000,  // 07:00 - capped peak
    4700000,  // 08:00
    3000000,  // 09:00

    2500000,  // 10:00 - steady daytime
    2300000,  // 11:00
    2400000,  // 12:00
    2300000,  // 13:00
    2500000,  // 14:00

    3000000,  // 15:00
    3700000,  // 16:00
    4700000,  // 17:00
    5000000,  // 18:00 - evening peak shaved
    5000000,  // 19:00 - max (capped)

    4000000,  // 20:00
    2800000,  // 21:00
    2000000,  // 22:00
    1500000   // 23:00
};

const long usage_profile_with_PV_BATT[24]{
    800000,    // 00:00 - night
    700000,    // 01:00
    650000,    // 02:00
    650000,    // 03:00
    750000,    // 04:00

    2000000,   // 05:00
    3000000,   // 06:00
    4500000,   // 07:00 - morning peak (battery assisted)
    4200000,   // 08:00
    2500000,   // 09:00

    1000000,   // 10:00 - PV generation begins
    -500000,   // 11:00 - excess stored in battery
    -1500000,  // 12:00 - peak PV generation (shaved from -4M)
    -1000000,  // 13:00
    -500000,   // 14:00
    500000,    // 15:00 - returning to net load

    3000000,   // 16:00
    4200000,   // 17:00
    4900000,   // 18:00 - peak
    5000000,   // 19:00 - capped

    4000000,   // 20:00
    2700000,   // 21:00
    2000000,   // 22:00
    1500000    // 23:00
};
