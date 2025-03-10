#include <stdio.h>
#include <stdlib.h>

#include "wally_core.h"
#include "wally_address.h"
#include "wally_descriptor.h"

#include "mutinynet.h"

// https://bitcoindevkit.github.io/book-of-bdk/cookbook/quickstart/
#define DESC_EXT    "tr([12071a7c/86'/1'/0']tpubDCaLkqfh67Qr7ZuRrUNrCYQ54sMjHfsJ4yQSGb3aBr1yqt3yXpamRBUwnGSnyNnxQYu7rqeBiPfw3mjBcFNX4ky2vhjj9bDrGstkfUbLB9T/0/*)#z3x5097m"
#define DEST_CHG    "tr([12071a7c/86'/1'/0']tpubDCaLkqfh67Qr7ZuRrUNrCYQ54sMjHfsJ4yQSGb3aBr1yqt3yXpamRBUwnGSnyNnxQYu7rqeBiPfw3mjBcFNX4ky2vhjj9bDrGstkfUbLB9T/1/*)#n9r4jswr"

#define STOP_GAP    (50)

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

int main(int argc, char *argv[])
{
    int rc;

    rc = wally_init(0);
    if (rc != WALLY_OK) {
        printf("error: wally_init fail: %d\n", rc);
        return 1;
    }

    descriptor();

    rc = wally_cleanup(0);
    if (rc != WALLY_OK) {
        printf("error: wally_cleanup fail: %d\n", rc);
        return 1;
    }
    return 0;
}
