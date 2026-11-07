/* Sync.rexx: FreeFileSync opens a saved configuration, compares, then syncs.
   rx Sync.rexx Work:Backup.ffs_gui   (FreeFileSync running) */
OPTIONS RESULTS
PARSE ARG config
IF ~SHOW('P', 'FREEFILESYNC.1') THEN DO
    SAY 'FreeFileSync is not running.'
    EXIT 5
END
ADDRESS 'FREEFILESYNC.1'
IF config ~= '' THEN 'OPEN' '"'config'"'
'COMPARE'
DO UNTIL RESULT = 0
    ADDRESS COMMAND 'Wait 1'
    'BUSY'
END
'SYNC'
SAY 'Synchronisation started.'
