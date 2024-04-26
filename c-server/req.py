#/usr/bin/env python

import requests

padding = "a" * 879
url = f"http://localhost:3000/{padding}"
r = requests.get(url)

print(r.status_code)
