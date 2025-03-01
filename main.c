#include <stdio.h>
#include <stdlib.h>

#include "wally_core.h"
#include "wally_address.h"
#include "wally_descriptor.h"

// https://bitcoindevkit.github.io/book-of-bdk/cookbook/quickstart/
#define DESC    "tr([12071a7c/86'/1'/0']tpubDCaLkqfh67Qr7ZuRrUNrCYQ54sMjHfsJ4yQSGb3aBr1yqt3yXpamRBUwnGSnyNnxQYu7rqeBiPfw3mjBcFNX4ky2vhjj9bDrGstkfUbLB9T/0/*)#z3x5097m"

static void descriptor(void)
{
    int rc;

    struct wally_descriptor *desc = NULL;

    rc = wally_descriptor_parse(
            DESC,
            NULL,
            WALLY_NETWORK_BITCOIN_TESTNET,
            0,
            &desc
    );
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_parse fail: %d\n", rc);
        goto cleanup;
    }

cleanup:
    if (desc) {
        wally_descriptor_free(desc);
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
