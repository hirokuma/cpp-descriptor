/*
 * $ git checkout refs/tags/release_1.4.0
 * $ ./configure --prefix=$HOME/.local --enable-minimal --disable-elements --enable-standard-secp --with-system-secp256k1 --disable-shared
 * $ make && make install
 */
#include <stdio.h>
#include <stdlib.h>

#include "wally_core.h"
#include "wally_address.h"
#include "wally_descriptor.h"
#include "wally_map.h"

#define KEY_1  "key_1"
#define KEY_2  "key_2"
#define KEY_3  "key_3"

// A 3-of-3 that turns into a 2-of-3 after 90 days
//  policy:     thresh(3,pk(key_1),pk(key_2),pk(key_3),older(12960))
//  miniscript: thresh(3,pk(key_1),s:pk(key_2),s:pk(key_3),sln:older(12960))
static void miniscript_test(void)
{
    int rc;
    struct wally_map *vars_in = NULL;
    struct wally_descriptor *desc = NULL;

    rc = wally_init(0);
    rc += wally_map_init_alloc(3, NULL, &vars_in);
    rc += wally_map_add(vars_in,
            (const uint8_t *)KEY_1, sizeof(KEY_1)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020201", 66);
    rc += wally_map_add(vars_in,
            (const uint8_t *)KEY_2, sizeof(KEY_2)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020202", 66);
    rc += wally_map_add(vars_in,
            (const uint8_t *)KEY_3, sizeof(KEY_3)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020203", 66);
    if (rc != WALLY_OK) {
        printf("error: wally_map_add fail: %d\n", rc);
        return;
    }

    rc = wally_descriptor_parse(
            "thresh(3,pk(key_1),s:pk(key_2),s:pk(key_3),sln:older(12960))",
            vars_in,
            WALLY_NETWORK_NONE,
            WALLY_MINISCRIPT_ONLY,
            &desc
    );
    if (rc != WALLY_OK) {
        printf("error: wally_descriptor_parse fail: %d\n", rc);
    }

    if (vars_in) {
        wally_map_free(vars_in);
    }
    if (desc) {
        wally_descriptor_free(desc);
    }
    wally_cleanup(0);
}

int main(int argc, char *argv[])
{
    miniscript_test();
    return 0;
}
