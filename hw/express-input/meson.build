


express_input = ss.source_set()

express_input.add(when: 'CONFIG_EXPRESS_INPUT',
               if_true: files(
                    'express_touchscreen.c',
                    'express_keyboard.c',
               ))




softmmu_ss.add_all(express_input)


