import numpy as np
import pylab as pl

o = [0.0010021630000665027, 0.002043746299978011, 0.0016078743799926088, 0.0015146685250022073, 0.0015701924091987166, 0.0016013072341799761, 0.0016934092575049996]
od = [0.004882193999947049, 0.0027009360997908515, 0.00115812690995881, 0.0006391752310319134, 0.0007622327519875398, 0.000729081031462265, 0.0006723820569174968]
p = [0.002330603000245901, 0.0021951264001472738, 0.0022849947500117197, 0.0024290702369976318, 0.0024169055240985927, 0.0024930315838701017, 0.002770220379807245]
pd = [0.0029706770001212135, 0.001510148299894354, 0.0013952666100976786, 0.0008594602829762152, 0.0007678150686007939, 0.0007526546974891335, 0.0007495576282087141]

run_counts = [1, 10, 100, 1000, 10000, 100000, 1000000]

pl.title('Average Runtime of coolprogram32')
pl.xlabel('Number of Program Executions')
pl.ylabel('Mean Runtime (seconds)')


pl.plot(run_counts, o, label='Original coolprogram32')
pl.plot(run_counts, od, label='Original coolprogram32 (no output)')
pl.plot(run_counts, p, label='Protected coolprogram32')
pl.plot(run_counts, pd, label='Protected coolprogram32 (no output)')

pl.legend()

pl.savefig('runtime_full.png', bbox_inches='tight')
pl.show()


# Plot for the first few counts
quick_counts = [1, 10, 100, 1000] #, 10000]

pl.title('Average Runtime of coolprogram32 - Quick Runs')
pl.xlabel('Number of Program Executions')
pl.ylabel('Mean Runtime (seconds)')


pl.plot(quick_counts, o[:4], label='Original coolprogram32')
pl.plot(quick_counts, od[:4], label='Original coolprogram32 (no output)')
pl.plot(quick_counts, p[:4], label='Protected coolprogram32')
pl.plot(quick_counts, pd[:4], label='Protected coolprogram32 (no output)')

pl.legend()

pl.savefig('runtime_quick.png', bbox_inches='tight')
pl.show()
