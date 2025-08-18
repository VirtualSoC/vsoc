/**
 * @file em_ruim.c
 * @author Haitao Su (haitaosu2002@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-8-9
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "qemu/osdep.h"
#include "qapi/qmp/qdict.h"
#include "qapi/qmp/qstring.h"
#include "qapi/qmp/qobject.h"

#include "hw/express-network/em_ruim.h"
#include <errno.h>
#include <string.h>

// #define STD_DEBUG_LOG
#include "hw/teleport-express/express_log.h"

typedef struct Ruim {
    QDict *qdict;
    QString *filename;
} Ruim;

static Ruim *ruim_create_empty(void)
{
    Ruim *ruim = (Ruim *)malloc(sizeof(Ruim));
    if (ruim) {
        ruim->qdict = qdict_new();
    }
    return ruim;
}

Ruim *ruim_create_from_file(const char *filename)
{
    Ruim *ruim = ruim_create_empty();
    if (!ruim) {
        return NULL;
    }
    ruim->filename = qstring_from_str(filename);

    FILE *file = fopen(filename, "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            char *key = strtok(line, "=");
            char *value = strtok(NULL, " \t\n\r");
            if (key && value) {
                LOGD("RUIM key=%s, value=%s", key, value);
                qdict_put_str(ruim->qdict, key, value);
            } else {
                LOGW("Key or value is invalid");
            }
        }
        fclose(file);
#ifdef _WIN32
    } else if (errno == ENOENT) {
        LOGW("Ruim file %s not exist, using empty ruim config", filename);
#endif
    } else {
        LOGE("Failed to open file %s: %s", filename, strerror(errno));
    }

    return ruim;
}

static void ruim_save_to_file(Ruim *ruim)
{
    if (!ruim || !ruim->qdict) {
        LOGW("!ruim || !ruim->qdict");
        return;
    }

    const char *filename = qstring_get_str(ruim->filename);
    FILE *file = fopen(filename, "w");
    if (file) {
        const QDictEntry *entry;
        for (entry = qdict_first(ruim->qdict); entry; entry = qdict_next(ruim->qdict, entry)) {
            const char *str = qstring_get_str(qobject_to(QString, qdict_entry_value(entry)));
            fprintf(file, "%s=%s\n", entry->key, str);
        }
        fclose(file);
    } else {
        LOGE("Failed to saving to ruim file %s: %s", filename, strerror(errno));
    }
}

int ruim_get_int(Ruim *ruim, const char *key, int defval)
{
    if (!ruim || !ruim->qdict) {
        LOGW("!ruim || !ruim->qdict");
        return RUIM_NOT_FOUND;
    }

    const char *str = qdict_get_try_str(ruim->qdict, key);
    if (str == NULL) {
        if (defval != RUIM_NOT_FOUND) {
            char buf[20];
            sprintf(buf, "%d", defval);
            ruim_set_str(ruim, key, buf);
        }
        return defval;
    } else {
        return atoi(str);
    }
}

const char *ruim_get_str(Ruim *ruim, const char *key, const char *defval)
{
    if (!ruim || !ruim->qdict) {
        LOGW("!ruim || !ruim->qdict");
        return NULL;
    }

    const char *str = qdict_get_try_str(ruim->qdict, key);
    if (str == NULL) {
        if (defval != NULL) {
            ruim_set_str(ruim, key, defval);
        }
        return defval;
    } else {
        return str;
    }
}

void ruim_set_str(Ruim *ruim, const char *key, const char *value)
{
    if (!ruim || !ruim->qdict) {
        LOGW("!ruim || !ruim->qdict");
        return;
    }

    if (value == NULL) {
        LOGW("value is NULL");
        return;
    }

    qdict_put_str(ruim->qdict, key, value);
    // XXX: It's not efficient to write back to ruim file every time we set key-value
    ruim_save_to_file(ruim);
}