


express_sensor = ss.source_set()

express_sensor.add(when: 'CONFIG_EXPRESS_SENSOR',
               if_true: files(
                    'express_battery.c',
                    'express_accel.c',
                    'express_gyro.c',
                    'express_gps.c',
                    'express_mic.c'
               ))




softmmu_ss.add_all(express_sensor)


