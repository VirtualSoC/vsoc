express_display = ss.source_set()

express_display.add(when: 'CONFIG_EXPRESS_DISPLAY',
    if_true: files(
        'express_display.c',
        'express_display_options.c',
    )
)

softmmu_ss.add_all(express_display)
vsoc_ss.add_all(express_display)
