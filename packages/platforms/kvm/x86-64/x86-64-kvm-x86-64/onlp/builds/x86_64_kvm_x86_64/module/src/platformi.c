/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *        Copyright 2014, 2015 Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
*/

#include <onlp/platformi/base.h>

const char*
onlp_platformi_get(void)
{
    return "x86-64-kvm-x86-64-r0";
}

int
onlp_platformi_sw_init(void)
{
    return ONLP_STATUS_OK;
}

int
onlp_platformi_manage_fans(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_platformi_manage_leds(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}
