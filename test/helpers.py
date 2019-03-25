# -*- coding: utf-8 -*-
#
import hashlib
import os
import pathlib
import requests
import shutil


def download(name, md5):

    filename = os.path.join("/tmp", name)
    if not os.path.exists(filename):
        print("Downloading {}...".format(name))
        url = "https://nschloe.github.io/pygalmesh/"
        print(url + name)
        r = requests.get(url + name, stream=True)
        if not r.ok:
            raise RuntimeError(
                "Download error ({}, return code {}).".format(r.url, r.status_code)
            )

        pathlib.Path(os.path.dirname(filename)).mkdir(parents=True, exist_ok=True)

        # save the mesh in /tmp
        with open(filename, "wb") as f:
            r.raw.decode_content = True
            shutil.copyfileobj(r.raw, f)

    # check MD5
    file_md5 = hashlib.md5(open(filename, "rb").read()).hexdigest()

    if file_md5 != md5:
        raise RuntimeError("Checksums not matching ({} != {}).".format(file_md5, md5))

    return filename
