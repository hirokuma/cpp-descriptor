#include <stdio.h>
#include <stdlib.h>

#include "wally_core.h"
#include "wally_address.h"
#include "wally_descriptor.h"
#include "wally_map.h"

#include "mutinynet.h"

// https://bitcoindevkit.github.io/book-of-bdk/cookbook/quickstart/
#define DESC_EXT    "tr([12071a7c/86'/1'/0']tpubDCaLkqfh67Qr7ZuRrUNrCYQ54sMjHfsJ4yQSGb3aBr1yqt3yXpamRBUwnGSnyNnxQYu7rqeBiPfw3mjBcFNX4ky2vhjj9bDrGstkfUbLB9T/0/*)#z3x5097m"
#define DEST_CHG    "tr([12071a7c/86'/1'/0']tpubDCaLkqfh67Qr7ZuRrUNrCYQ54sMjHfsJ4yQSGb3aBr1yqt3yXpamRBUwnGSnyNnxQYu7rqeBiPfw3mjBcFNX4ky2vhjj9bDrGstkfUbLB9T/1/*)#n9r4jswr"

#define STOP_GAP    (50)

static void dump(const uint8_t *data, size_t sz)
{
    for (size_t i = 0; i < sz; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}


static void descriptor(void)
{
    int rc;

    struct wally_descriptor *desc_ext = NULL;
    struct wally_descriptor *desc_chg = NULL;

    int cnt;
    int not_detect_cnt;

    // external
    rc = wally_descriptor_parse(
            DESC_EXT,
            NULL,
            WALLY_NETWORK_BITCOIN_TESTNET,
            0,
            &desc_ext
    );
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_parse fail: %d\n", rc);
        goto cleanup;
    }

    cnt = 0;
    not_detect_cnt = 0;
    while (1) {
        char *output = NULL;
        rc = wally_descriptor_to_address(
                desc_ext,
                0,      // variant
                0,      // multi_index
                cnt,    // child_num
                0,      // flags
                &output);
        if (rc != WALLY_OK) {
            printf("error: wally_descriptor_to_address fail: %d\n", rc);
            goto cleanup;
        }
        bool detect = mutiny_detect_address(output);
        if (!detect) {
            not_detect_cnt++;
            if (not_detect_cnt == 1) {
                printf("output[ex %d]: %s (generated)\n", cnt, output);
            }
        } else {
            not_detect_cnt = 0;
            printf("output[ex %d]: %s\n", cnt, output);
        }
        wally_free_string(output);

        if (not_detect_cnt == STOP_GAP) {
            break;
        }
        cnt++;
    }
    rc = wally_descriptor_parse(
            DESC_EXT,
            NULL,
            WALLY_NETWORK_BITCOIN_TESTNET,
            0,
            &desc_ext
    );
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_parse fail: %d\n", rc);
        goto cleanup;
    }

    // change
    rc = wally_descriptor_parse(
            DEST_CHG,
            NULL,
            WALLY_NETWORK_BITCOIN_TESTNET,
            0,
            &desc_chg
    );
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_parse(chg) fail: %d\n", rc);
        goto cleanup;
    }

    cnt = 0;
    not_detect_cnt = 0;
    while (1) {
        char *output = NULL;
        rc = wally_descriptor_to_address(
                desc_chg,
                0,      // variant
                0,      // multi_index
                cnt,    // child_num
                0,      // flags
                &output);
        if (rc != WALLY_OK) {
            printf("error: wally_descriptor_to_address fail(chg): %d\n", rc);
            goto cleanup;
        }
        bool detect = mutiny_detect_address(output);
        if (!detect) {
            not_detect_cnt++;
            if (not_detect_cnt == 1) {
                printf("output[in %d]: %s (generated)\n", cnt, output);
            }
        } else {
            not_detect_cnt = 0;
            printf("output[in %d]: %s\n", cnt, output);
        }
        wally_free_string(output);

        if (not_detect_cnt == STOP_GAP) {
            break;
        }
        cnt++;
    }

cleanup:
    if (desc_ext) {
        wally_descriptor_free(desc_ext);
    }
}

static void miniscript(void)
{
    int rc;

    struct wally_map *vars_in = NULL;
    struct wally_descriptor *ms = NULL;
    uint8_t script[256];
    size_t sz = 0;

    rc = wally_map_init_alloc(4, NULL, &vars_in);
    if (rc != WALLY_OK) {
        printf("error: wally_map_init fail: %d\n", rc);
        goto cleanup;
    }
#define KEY_1  "key_1"
#define KEY_2  "key_2"
#define KEY_3  "key_3"

    rc = wally_map_add(vars_in,
            (const uint8_t *)KEY_1, sizeof(KEY_1)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020201", 66);
    if (rc != WALLY_OK) {
        printf("error: wally_map_add fail: %d\n", rc);
        goto cleanup;
    }
    rc = wally_map_add(vars_in,
            (const uint8_t *)KEY_2, sizeof(KEY_2)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020202", 66);
    if (rc != WALLY_OK) {
        printf("error: wally_map_add fail: %d\n", rc);
        goto cleanup;
    }
    rc = wally_map_add(vars_in,
            (const uint8_t *)KEY_3, sizeof(KEY_3)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020203", 66);
    if (rc != WALLY_OK) {
        printf("error: wally_map_add fail: %d\n", rc);
        goto cleanup;
    }
    rc = wally_map_add(vars_in,
            (const uint8_t *)"H", 1,
            (const uint8_t *)"0303030303030303030303030303030303030303", 40);
    if (rc != WALLY_OK) {
        printf("error: wally_map_add fail: %d\n", rc);
        goto cleanup;
    }

    rc = wally_descriptor_parse(
            // "thresh(2,pk(key_1),s:pk(key_2),sln:older(12960))", //NG
            "t:or_c(pk(key_1),and_v(v:pk(key_2),or_c(pk(key_3),v:hash160(H))))",    //OK
            // "andor(pk(key_1),or_i(and_v(v:pkh(key_2),hash160(H)),older(1008)),pk(key_3))",   //OK
            vars_in,
            WALLY_NETWORK_NONE,
            WALLY_MINISCRIPT_ONLY,
            &ms
    );
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_parse fail: %d\n", rc);
        goto cleanup;
    }

    rc = wally_descriptor_to_script(
        ms,
        0,      // depth
        0,      // index
        0,      // variant
        0,      // multi_index
        0,      // child_num
        0,      // flags
        script, sizeof(script), &sz);
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_to_script fail: %d\n", rc);
        goto cleanup;
    }
    printf("script=");
    dump(script, sz);

cleanup:
    if (vars_in) {
        wally_map_free(vars_in);
    }
    if (ms) {
        wally_descriptor_free(ms);
    }
}

int main(int argc, char *argv[])
{
    int rc;

    rc = wally_init(0);
    if (rc != WALLY_OK) {
        printf("error: wally_init fail: %d\n", rc);
        return 1;
    }

    //descriptor();
    miniscript();

    rc = wally_cleanup(0);
    if (rc != WALLY_OK) {
        printf("error: wally_cleanup fail: %d\n", rc);
        return 1;
    }
    return 0;
}
