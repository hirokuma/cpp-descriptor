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

#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

#define KEY_1  "key_1"
#define KEY_2  "key_2"
#define KEY_3  "key_3"
#define H       "H"

// A 3-of-3 that turns into a 2-of-3 after 90 days
//  policy:     thresh(3,pk(key_1),pk(key_2),pk(key_3),older(12960))
//  miniscript: thresh(3,pk(key_1),s:pk(key_2),s:pk(key_3),sln:older(12960))
static void miniscript_test(const char *mini)
{
    int rc;
    struct wally_map *vars_in = NULL;
    struct wally_descriptor *desc = NULL;

    rc = wally_init(0);
    rc += wally_map_init_alloc(4, NULL, &vars_in);
    rc += wally_map_add(vars_in,
            (const uint8_t *)KEY_1, sizeof(KEY_1)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020201", 66);
    rc += wally_map_add(vars_in,
            (const uint8_t *)KEY_2, sizeof(KEY_2)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020202", 66);
    rc += wally_map_add(vars_in,
            (const uint8_t *)KEY_3, sizeof(KEY_3)-1,
            (const uint8_t *)"020202020202020202020202020202020202020202020202020202020202020203", 66);
    rc += wally_map_add(vars_in,
            (const uint8_t *)H, sizeof(H)-1,
            (const uint8_t *)"0202020202020202020202020202020202020202", 40);
    if (rc != WALLY_OK) {
        printf("error: wally_map_add fail: %d\n", rc);
        return;
    }

    printf("miniscript: %s\n", mini);
    rc = wally_descriptor_parse(
            mini,
            vars_in,
            WALLY_NETWORK_NONE,
            WALLY_MINISCRIPT_ONLY,
            &desc
    );
    if (rc == WALLY_OK) {
        printf("OK\n");
    } else {
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
    // https://bitcoin.sipa.be/miniscript/
    const char *TEST_SCRIPTS[] = {
        // A single key
        "pk(key_1)",
        // One of two keys (equally likely)
        "or_b(pk(key_1),s:pk(key_2))",
        // One of two keys (one likely, one unlikely)
        "or_d(pk(key_1),pkh(key_2))",
        // A user and a 2FA service need to sign off, but after 90 days the user alone is enough
        "and_v(v:pk(key_1),or_d(pk(key_2),older(12960)))",
        // A 3-of-3 that turns into a 2-of-3 after 90 days
        "thresh(3,pk(key_1),s:pk(key_2),s:pk(key_3),sln:older(12960))",
        // The BOLT #3 to_local policy
        "andor(pk(key_1),older(1008),pk(key_2))",
        // The BOLT #3 offered HTLC policy
        "t:or_c(pk(key_1),and_v(v:pk(key_2),or_c(pk(key_3),v:hash160(H))))",
        // The BOLT #3 received HTLC policy
        "andor(pk(key_1),or_i(and_v(v:pkh(key_2),hash160(H)),older(1008)),pk(key_3))",

    };
    for (size_t i = 0; i < ARRAY_SIZE(TEST_SCRIPTS); i++) {
        miniscript_test(TEST_SCRIPTS[i]);
    }
    return 0;
}
