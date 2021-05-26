import subprocess
import os
class OnlServiceMixin(object):
    def _execute(self, cmd, root=False, ex=True):
        self.logger.debug("Executing: %s" % cmd)
        if root is True and os.getuid() != 0:
            cmd = "sudo " + cmd
        try:
            subprocess.check_call(cmd, shell=True)
        except Exception as e:
            if ex:
                self.logger.error("Command failed: %s" % e)
                raise
            else:
                return e.returncode

    def _raise(self, msg, klass):
        self.logger.critical(msg)
        raise klass(msg)


def dmerge(d1, d2):
    """
    dictionary merge.

    d1 is the default source. Leaf values from d2 will override.

    d1 is the 'default' source; leaf values from d2 will override.
    Returns the merged tree.

    Set a leaf in d2 to None to create a tombstone (discard any key
    from d1).

    if a (sub) key in d1, d2 differ in type (dict vs. non-dict) then
    the merge will proceed with the non-dict promoted to a dict using
    the default-key schema ('='). Consumers of this function should be
    prepared to handle such keys.
    """
    merged = {}
    q = [(d1, d2, merged)]
    while True:
        if not q: break
        c1, c2, c3 = q.pop(0)
        # add in non-overlapping keys
        # 'None' keys from p2 are tombstones
        s1 = set(c1.keys())
        s2 = set(c2.keys())

        for k in s1.difference(s2):
            v = c1[k]
            if type(v) == dict:
                c3.setdefault(k, {})
                q.append((v, {}, c3[k],))
            else:
                c3.setdefault(k, v)

        for k in s2.difference(s1):
            v = c2[k]
            if v is None: continue
            if type(v) == dict:
                c3.setdefault(k, {})
                q.append(({}, v, c3[k],))
            else:
                c3.setdefault(k, v)

        # handle overlapping keys
        for k in s1.intersection(s2):
            v1 = c1[k]
            v2 = c2[k]

            if v2 is None: continue

            # two dicts, key-by-key reconciliation required
            if type(v1) == dict and type(v2) == dict:
                c3.setdefault(k, {})
                q.append((v1, v2, c3[k],))
                continue

            # two non-dicts, p2 wins
            if type(v1) != dict and type(v2) != dict:
                c3[k] = v2
                continue

            if type(v1) != dict:
                v1 = { '=' : v1, }
            if type(v2) != dict:
                v2 = { '=' : v2, }
            c3.setdefault(k, {})
            q.append((v1, v2, c3[k],))

    return merged
