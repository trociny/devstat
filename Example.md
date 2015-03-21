Under FreeBSD devstat info displayed looks like this:

```

kopusha:~% devstat 
ad0:
        29184 bytes read
        0 bytes written
        0 bytes freed
        39 reads
        0 writes
        0 frees
        0 other
        duration:
                0 8271858279415856098/2^64 sec reads
                0 0/2^64 sec writes
                0 0/2^64 sec frees
        0 8271858279415856098/2^64 sec busy time
        1 17316077238080354385/2^64 sec creation time
        512 block size
        tags sent:
                39 simple
                0 ordered
                0 head of queue
        supported statisctics measurments flags: 0
        device type: 0
        devstat list insert priority: 4095
ad2:
        45231104 bytes read
        25548800 bytes written
        0 bytes freed
        2687 reads
        1857 writes
        0 frees
        0 other
        duration:
                25 4629847552146137086/2^64 sec reads
                23 7494645774336775744/2^64 sec writes
                0 0/2^64 sec frees
        22 12949347393270322518/2^64 sec busy time
        2 456161826735997133/2^64 sec creation time
        512 block size
        tags sent:
                4544 simple
                0 ordered
                0 head of queue
        supported statisctics measurments flags: 0
        device type: 0
        devstat list insert priority: 4095

```

Under NetBSD there are less fields:

```

$ ./devstat
xbd0:
        device type: 0
        busy counter: 0
        405659136 bytes read
        552335872 bytes written
        21883 reads
        41602 writes
        41602 seeks
        22.901264 sec spent busy
        attach timestamp: 1.338699 sec
        unbusy timestamp: 166747.148681 sec
xbd1:
        device type: 0
        busy counter: 0
        3584 bytes read
        0 bytes written
        6 reads
        0 writes
        0 seeks
        0.029997 sec spent busy
        attach timestamp: 1.338699 sec
        unbusy timestamp: 1.368696 sec
md0:
        device type: 0
        busy counter: 0
        0 bytes read
        0 bytes written
        0 reads
        0 writes
        0 seeks
        0.000000 sec spent busy
        attach timestamp: 1.338699 sec
        unbusy timestamp: 0.000000 sec
nfs0:
        device type: 0
        busy counter: 0
        34664956 bytes read
        0 bytes written
        1059 reads
        0 writes
        0 seeks
        1.719998 sec spent busy
        attach timestamp: 164803.028681 sec
        unbusy timestamp: 164885.968681 sec

```