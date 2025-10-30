#!/usr/bin/env python3
# -*- coding: utf-8 -*-

csv_content = """email,passwordHash,fullName,birthDate,phone,registeredAt
ntctuyen@gmail.com,17120e13a8b2a45,Nguyễn Thị Cẩm Tuyền,09/07/2006,0898234406,1761397390
test@gmail.com,85c23cd3e08d3d74,Test User,01/01/2000,0901234567,1761396484
vanvu41ht3@gmail.com,25fe1f608c4300be,Nguyễn Vi,07/06/2006,0335912027,1761573201
vanvu41ht4@gmail.com,24f1eba07df13783,Nguyễn Văn Vũ,07/06/2006,0335912027,1761723938
tuyen@gmail.com,b2ccabda82cb688,Tuyền,09/07/2006,0898234406,1761403265
"""

with open(r'c:\Users\Cam Tuyen\Desktop\PBL2\data\users.csv', 'w', encoding='utf-8-sig') as f:
    f.write(csv_content)

print("✅ File CSV đã được tạo với UTF-8 BOM!")
