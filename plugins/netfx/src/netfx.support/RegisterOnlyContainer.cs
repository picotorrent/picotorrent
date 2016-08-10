using System;
using TinyIoC;

namespace PicoTorrent
{
    public sealed class RegisterOnlyContainer : IContainer
    {
        private readonly TinyIoCContainer _container;

        public RegisterOnlyContainer(TinyIoCContainer container)
        {
            if (container == null) throw new ArgumentNullException(nameof(container));
            _container = container;
        }

        public void Register<TContract, TImplementation>()
            where TContract : class
            where TImplementation : class, TContract
        {
            _container.Register<TContract, TImplementation>();
        }

        public void RegisterSingleton<TContract, TImplementation>()
            where TContract : class
            where TImplementation : class, TContract
        {
            _container.Register<TContract, TImplementation>().AsSingleton();
        }
    }
}
