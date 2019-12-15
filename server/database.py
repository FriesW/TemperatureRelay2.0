import sqlite3
import time
from config import DATABASE_FILE

__con = sqlite3.connect(DATABASE_FILE)
__cur = __con.cursor()

__cur.execute("CREATE TABLE IF NOT EXISTS measurements (time INTEGER PRIMARY KEY, temperature INTEGER NOT NULL, creation_time INTEGER NOT NULL) WITHOUT ROWID")

def entry_count():
    for i in __cur.execute("SELECT COUNT(*) FROM measurements"):
        return i[0]

def insert(epoch, temp):
    try:
        epoch = int(epoch)
        temp = int(temp)
        __cur.execute("INSERT INTO measurements VALUES (?, ?, ?)", (epoch, temp, time.time()) )
        __con.commit()
        return True
    except:
        return False

def iterate_recent(time_cutoff=0, limit=10000, descending = True):
    limit = int(limit)
    limit = min(limit, 1000000)
    yield from __cur.execute(
        "SELECT * FROM (SELECT time, temperature FROM measurements WHERE time > {} ORDER BY time DESC LIMIT {}) sq ORDER BY time {}"\
        .format(time_cutoff, limit, 'DESC' if descending else 'ASC')
        )

def most_recent():
    for i in __cur.execute("SELECT time, temperature FROM measurements ORDER BY time DESC LIMIT 1"):
        return i

def min_temp(time_cutoff=0):
    for i in __cur.execute("SELECT MIN(temperature) FROM measurements WHERE time > {}".format(time_cutoff)):
        return i[0]

def debug_print():
    now = time.time() + 60 * 60 * 24 * 3.2
    fs = "inserted in the past by: {: >4d} d  {: >2d} h  {: >2d} m  {: >6.3f} s    time: {: >15,}     raw_temp: {: >6,}    deg C: {: >3.4f}"
    for row in __cur.execute("SELECT * FROM measurements ORDER BY creation_time DESC LIMIT 1000"):
        delta = now - row[2]
        d, delta = divmod( delta, 60 * 60 * 24 )
        h, delta = divmod( delta, 60 * 60 )
        m, s     = divmod( delta, 60 )
        print(fs.format(int(d), int(h), int(m), s, row[0], row[1], row[1] / 128))

def testing_data():
    import random
    end = int(time.time())
    start = end - 60 * 60 * 24 * 35 #35 days ago
    for ti in range(start, end, 60 * 10):
        insert(ti, random.randint(500, 4000))
